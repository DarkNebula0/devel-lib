#include "TCPServer.h"
#include "TCPSocket.h"
#include "TCPTask.h"

#include <WS2tcpip.h>
#include <MSWSock.h>


constexpr DWORD g_nAddressLength = sizeof(sockaddr_in) + 16;


Network::TCP::CServer::CServer()
	:	m_fnAcceptEx(nullptr), m_fIsRunning(false), m_fIsAccepting(false), m_pIocp(nullptr),
		m_pAcceptTask(std::make_unique<Network::TCP::SAcceptTask>()),
		m_iReceiveBufferSize(TCP_SERVER_BUFFER_SIZE_DEFAULT), m_iMaxConnections(TCP_SERVER_MAX_CONNECTIONS_DEFAULT), m_iThreadCount(TCP_SERVER_THREADS_DEFAULT)
{
	this->setSocketClass<Network::TCP::CSocket>();

	this->m_oHandler.setAcceptCallback(std::bind(&CServer::onAccept, this, std::placeholders::_1));
	this->m_oHandler.setRecvCallback(std::bind(&CServer::onRecv, this, std::placeholders::_1, std::placeholders::_2));
	this->m_oHandler.setSendCallback(std::bind(&CServer::onSend, this, std::placeholders::_1, std::placeholders::_2));
	this->m_oHandler.setDisconnectCallback(std::bind(&CServer::onDisconnect, this, std::placeholders::_1));
}

Network::TCP::CServer::~CServer()
{
	this->stop();
}


Network::EError Network::TCP::CServer::start(const bool i_fExecute)
{
	if (this->m_fIsRunning)
	{
		return Network::EError::ESuccess;
	}

	if (!this->isValid())
	{
		const Network::EError eError = this->create(Network::ESocketProtocol::SPTCP);
		if (eError != Network::EError::ESuccess)
		{
			return eError;
		}
	}

	{
		const Network::EError eError = this->bind();
		if (eError != Network::EError::ESuccess)
		{
			return eError;
		}
	}

	{
		const Network::EError eError = this->retrieveFunctions();
		if (eError != Network::EError::ESuccess)
		{
			return eError;
		}
	}

	if (this->m_pIocp)
	{
		CloseHandle(this->m_pIocp);
	}

	this->m_pIocp = this->createIocp(true, this->m_iThreadCount);
	if (this->m_pIocp == nullptr)
	{
		return Network::EError::EWSALastError;
	}

	if (::listen(Network::CSocket::socket(), 1) == SOCKET_ERROR)
	{
		return Network::EError::EWSALastError;
	}

	this->m_oHandler.setCompletionPort(this->m_pIocp);
	this->m_fIsRunning = true;

	if (i_fExecute)
	{
		this->execute();
	}

	return Network::EError::ESuccess;
}


void Network::TCP::CServer::execute()
{
	this->startAccept();
	this->executeThreadPool();
}


void Network::TCP::CServer::stop()
{
	if (this->m_fIsRunning)
	{
		Network::CSocket::close();

		ForEachMV(const TCPSocket &pSocket, this->m_apSockets)
		{
			pSocket->close();
			pSocket->onDisconnected();
			this->onDisconnected(pSocket);
		}
		this->m_apSockets.clear();

		this->m_fIsRunning = false;
	}

	if (this->m_oThreadPool.isExecuted())
	{
		this->m_oThreadPool.stop();
	}

	if (this->m_pIocp)
	{
		CloseHandle(this->m_pIocp);
		this->m_pIocp = nullptr;
		this->m_oHandler.setCompletionPort(nullptr);
	}

	this->m_fIsAccepting = false;
}


TCPSocket Network::TCP::CServer::socket(Network::SOCKET i_nSocket) const
{
	try
	{
		return this->m_apSockets.get([i_nSocket](const TCPSocket &i_pSocket)
		{
			return (i_pSocket->socket() == i_nSocket);
		});
	} catch (const CNoEntryFoundException &)
	{ }

	return nullptr;
}

Network::EError Network::TCP::CServer::retrieveFunctions()
{
	this->m_fnAcceptEx = this->retrieveFunction(WSAID_ACCEPTEX);
	if (!this->m_fnAcceptEx)
	{
		return Network::EError::EWSALastError;
	}

	this->m_fnGetAcceptExSockAddrs = this->retrieveFunction(WSAID_GETACCEPTEXSOCKADDRS);
	if (!this->m_fnGetAcceptExSockAddrs)
	{
		return Network::EError::EWSALastError;
	}

	return Network::EError::ESuccess;
}


void Network::TCP::CServer::startAccept()
{
	if (this->m_fIsAccepting || this->m_apSockets.size() >= this->m_iMaxConnections)
	{
		return;
	}

	this->m_fIsAccepting = true;

	memset(this->m_pAcceptTask.get(), 0, sizeof(Network::TCP::SAcceptTask));

	const LPFN_ACCEPTEX fnAcceptEx = static_cast<LPFN_ACCEPTEX>(this->m_fnAcceptEx);

	const SOCKET nAcceptSocket = ::socket(AF_INET, this->protocolData().iType, this->protocolData().iProtocol);
	if (!fnAcceptEx(Network::CSocket::socket(), nAcceptSocket, this->m_pAcceptTask->pBuffer, 0, g_nAddressLength, g_nAddressLength, &this->m_pAcceptTask->iProcessedBytes, reinterpret_cast<LPWSAOVERLAPPED>(this->m_pAcceptTask.get())))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			// ToDo: Error
			return;
		}
	}

	this->m_pAcceptTask->nSocket = nAcceptSocket;
	this->m_pAcceptTask->eTask = Network::TCP::ETask::TAccept;

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(nAcceptSocket), this->m_pIocp, 0, 0);
}

void Network::TCP::CServer::executeThreadPool()
{
	this->m_oThreadPool.execute(this->m_iThreadCount);
	for (uint i = 0; i < this->m_iThreadCount; i++)
	{
		this->m_oThreadPool.addTask(std::bind(&CHandler::handle, &this->m_oHandler, std::ref(this->m_fIsRunning)));
	}
}


void Network::TCP::CServer::onAccept(Network::TCP::SAcceptTask *i_pTask)
{
	Network::SPeer oRemotePeer;
	const SOCKET nSocket = i_pTask->nSocket;

	{ // Get Peer information (Ip, Port)
		const LPFN_GETACCEPTEXSOCKADDRS fnGetAcceptExSockAddrs = static_cast<LPFN_GETACCEPTEXSOCKADDRS>(this->m_fnGetAcceptExSockAddrs);
		Network::SPeer *pLocalPeer, *pRemotePeer;
		int iLocalPeerSize, iRemotePeerSize;

		fnGetAcceptExSockAddrs(i_pTask->pBuffer, 0, g_nAddressLength, g_nAddressLength, reinterpret_cast<sockaddr **>(&pLocalPeer), &iLocalPeerSize, reinterpret_cast<sockaddr **>(&pRemotePeer), &iRemotePeerSize);

		oRemotePeer = *pRemotePeer;
	}

	// Start new accept
	this->m_fIsAccepting = false;
	this->startAccept();

	// Socket-Creation
	const TCPSocket pSocket = this->m_oSocketClassData.constructShared<Network::TCP::CSocket>();
	pSocket->setAddress(oRemotePeer);
	pSocket->setSocket(nSocket);
	pSocket->setIsNonBlocking(true);

	this->m_apSockets.push_back(pSocket);

	this->onNewConnection(pSocket);
	pSocket->startRecv(this->m_iReceiveBufferSize);
}

void Network::TCP::CServer::onRecv(const Network::SOCKET i_nSocket, Network::TCP::SRecvTask *i_pTask)
{
	if (TCPSocket pSocket = this->socket(i_nSocket))
	{
		// When pTask is not set, it's the post-call (For restarting recv)
		if (i_pTask)
		{
			const Network::CPacket oPacket(i_pTask->oBuf.buf, static_cast<size_t>(i_pTask->iProcessedBytes));

			pSocket->onPacketReceived(oPacket);
			this->onPacketReceived(pSocket, oPacket);
		}
		else if (pSocket->isValid())
		{
			pSocket->startRecv(this->m_iReceiveBufferSize);
		}
	}
}

void Network::TCP::CServer::onSend(const Network::SOCKET i_nSocket, Network::TCP::SSendTask *i_pTask) const
{
	if (const TCPSocket pSocket = this->socket(i_nSocket))
	{
		if (i_pTask->iBytesSent < i_pTask->oUseBuf.oBuf.len)
		{
			const uint iLeftSize = static_cast<uint>(i_pTask->oUseBuf.oBuf.len - i_pTask->iBytesSent);

			pSocket->send(i_pTask->oUseBuf.oBuf.buf + i_pTask->iBytesSent, iLeftSize, static_cast<ESendFlag>(i_pTask->iFlags));
		}
	}
}

void Network::TCP::CServer::onDisconnect(const Network::SOCKET i_nSocket)
{
	if (TCPSocket pSocket = this->socket(i_nSocket))
	{
		pSocket->onDisconnected();
		this->onDisconnected(pSocket);
	}

	this->startAccept();
}