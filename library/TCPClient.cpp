#include "TCPClient.h"
#include "TCPTask.h"
#include "Timer.h"

#include <winsock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>


Network::TCP::CClient::CClient()
	: m_fnConnectEx(nullptr), m_eState(EState::SDisconnected), m_oIocp(nullptr), m_iReceiveBufferSize(TCP_SERVER_BUFFER_SIZE_DEFAULT), m_wSecondsConnectTimeout(0)
{
	this->m_oHandler.setConnectCallback(std::bind(&CClient::onConnect, this, std::placeholders::_1));
	this->m_oHandler.setRecvCallback(std::bind(&CClient::onRecv, this, std::placeholders::_1, std::placeholders::_2));
	this->m_oHandler.setSendCallback(std::bind(&CClient::onSend, this, std::placeholders::_1, std::placeholders::_2));
	this->m_oHandler.setDisconnectCallback(std::bind(&CClient::onDisconnect, this, std::placeholders::_1));
}

Network::EError Network::TCP::CClient::connect(const ushort i_wSecondsTimeout, const bool i_fExecute)
{
	if (this->m_eState != EState::SDisconnected)
	{
		return Network::EError::ESuccess;
	}

	if (!this->isValid()) {
		const Network::EError eError = this->create(ESocketProtocol::SPTCP);
		if (eError != Network::EError::ESuccess) {
			return eError;
		}

		if (this->isNotBlocked())
		{
			this->m_fnConnectEx = this->retrieveFunction(WSAID_CONNECTEX);
			this->m_oBindPeer.setAddress("0.0.0.0", 0);

			if (::bind(this->socket(), reinterpret_cast<const sockaddr *>(&this->m_oBindPeer.address()), sizeof(this->m_oBindPeer.address())) != 0)
			{
				this->stop();
				return Network::EError::EWSALastError;
			}
		}
	}

#pragma region Non Blocked
	if (this->isNotBlocked())
	{
		if (!this->m_oIocp)
		{
			this->m_oIocp = this->createIocp(true, 1);
			if (!this->m_oIocp)
			{
				return Network::EError::EWSALastError;
			}
		}

		this->m_wSecondsConnectTimeout = (i_wSecondsTimeout > 0 ? (i_wSecondsTimeout - 1) : i_wSecondsTimeout);
		this->m_oHandler.setCompletionPort(this->m_oIocp);
		return this->startConnect(i_fExecute);
	}
#pragma endregion

#pragma region Blocked
	const timeval oTimeout = { SECONDS(i_wSecondsTimeout > 0 ? (i_wSecondsTimeout > 1 ? 2 : 1) : 0), 0 };
	CTimer oTimer(true);

	this->m_eState = EState::SConnecting;
	do
	{
		const int iError = ::connect(this->socket(), reinterpret_cast<const sockaddr *>(&this->address()), sizeof(this->address()));
		if (iError != INVALID_SOCKET || WSAGetLastError() == WSAEWOULDBLOCK)
		{
			fd_set oSet;
			FD_ZERO(&oSet);
			FD_SET(this->socket(), &oSet);

			if (select(0, &oSet, &oSet, nullptr, &oTimeout) != 0)
			{
				this->m_eState = EState::SConnected;
				return Network::EError::ESuccess;
			}
		}
	} while (!oTimer.hasExpired(i_wSecondsTimeout));

	this->m_eState = EState::SDisconnected;

	return (WSAGetLastError() == 0 ? Network::EError::ETimeout : Network::EError::EWSALastError);
#pragma endregion
}

void Network::TCP::CClient::execute()
{
	if (!this->m_fIsExecuted)
	{
		this->m_fIsExecuted = true;
		this->m_oHandleThread = std::thread(&CHandler::handle, &this->m_oHandler, std::ref(this->m_fIsExecuted));
	}
}

void Network::TCP::CClient::stop()
{
	this->m_fIsExecuted = false;

	if (this->isValid())
	{
		Network::TCP::CSocket::close();
	}

	this->m_eState = EState::SDisconnected;

	if (this->m_oHandleThread.joinable())
	{
		this->m_oHandleThread.join();
	}

	if (this->m_oIocp)
	{
		CloseHandle(this->m_oIocp);
		this->m_oIocp = nullptr;
		this->m_oHandler.setCompletionPort(nullptr);
	}
}


Network::EError Network::TCP::CClient::startConnect(const bool i_fExecute)
{
	Network::TCP::SConnectTask *pTask = new Network::TCP::SConnectTask();
	memset(pTask, 0, sizeof(Network::TCP::SConnectTask));
	pTask->eTask = ETask::TConnect;
	pTask->nSocket = this->socket();

	const LPFN_CONNECTEX fnConnectEx = static_cast<LPFN_CONNECTEX>(this->m_fnConnectEx);
	if (!fnConnectEx(this->socket(), reinterpret_cast<const sockaddr *>(&this->address()), sizeof(this->address()), nullptr, 0, nullptr, reinterpret_cast<LPWSAOVERLAPPED>(pTask)) &&
		WSAGetLastError() != WSA_IO_PENDING)
	{
		delete pTask;
		return Network::EError::EWSALastError;
	}

	this->m_eState = EState::SConnecting;

	if (i_fExecute)
	{
		this->execute();
	}

	return Network::EError::ESuccess;
}


void Network::TCP::CClient::onConnected()
{
	if (this->m_fnConnected)
	{
		return this->m_fnConnected();
	}
}

void Network::TCP::CClient::onConnectFailed()
{
	if (this->m_fnConnectFailed)
	{
		return this->m_fnConnectFailed();
	}
}

void Network::TCP::CClient::onPacketReceived(const Network::CPacket &i_oPacket)
{
	if (this->m_fnRecv)
	{
		return this->m_fnRecv(i_oPacket);
	}
}

void Network::TCP::CClient::onDisconnected()
{
	if (this->m_fnDisconnected)
	{
		this->m_fnDisconnected();
	}
}


void Network::TCP::CClient::onConnect(Network::TCP::SConnectTask *i_pTask)
{
	if (i_pTask) // Successfully connected
	{
		setsockopt(this->socket(), SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, nullptr, 0);
		this->m_eState = EState::SConnected;
		this->onConnected();
		this->startRecv(this->m_iReceiveBufferSize);
	}
	else // Connect failed
	{
		if (this->m_wSecondsConnectTimeout > 0)
		{
			this->m_wSecondsConnectTimeout--;
			this->startConnect(false);
		} else
		{
			this->onConnectFailed();
		}
	}
}

void Network::TCP::CClient::onRecv(const Network::SOCKET i_nSocket, Network::TCP::SRecvTask *i_pTask)
{
	// When pTask is not set, it's the post-call (For restarting recv)
	if (i_pTask)
	{
		const Network::CPacket oPacket(i_pTask->oBuf.buf, static_cast<size_t>(i_pTask->iProcessedBytes));

		this->onPacketReceived(oPacket);
	}
	else if (this->isValid())
	{
		this->startRecv(this->m_iReceiveBufferSize);
	}
}

void Network::TCP::CClient::onSend(const Network::SOCKET i_nSocket, Network::TCP::SSendTask *i_pTask) const
{
	if (i_pTask->iBytesSent < i_pTask->oUseBuf.oBuf.len)
	{
		const uint iLeftSize = static_cast<uint>(i_pTask->oUseBuf.oBuf.len - i_pTask->iBytesSent);

		this->send(i_pTask->oUseBuf.oBuf.buf + i_pTask->iBytesSent, iLeftSize, static_cast<ESendFlag>(i_pTask->iFlags));
	}
}

void Network::TCP::CClient::onDisconnect(const Network::SOCKET i_nSocket)
{
	this->m_eState = EState::SDisconnected;
	this->onDisconnected();
}