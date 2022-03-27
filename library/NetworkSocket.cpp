#include "NetworkSocket.h"
#include "WsaGlobal.h"

#include <WS2tcpip.h>


Network::CSocket::CSocket()
	: m_nSocket(INVALID_SOCKET), m_fIsNonBlocking(true)
{
	InitializeWSA();

	this->clear();
}


HANDLE Network::CSocket::createIocp(const bool i_fBindSocket, const uint i_iThreadCount) const
{
	if (this->isValid())
	{
		HANDLE pIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, i_iThreadCount);
		if (pIocp != nullptr && i_fBindSocket)
		{
			if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(this->socket()), pIocp, 0, 0) != pIocp)
			{
				CloseHandle(pIocp);
				pIocp = nullptr;
			}
		}

		return pIocp;
	}

	return nullptr;
}

Network::SProtocolData Network::CSocket::getProtocolData(const ESocketProtocol i_eProtocol)
{
	Network::SProtocolData oProtocol;
	switch (i_eProtocol)
	{
	case ESocketProtocol::SPTCP:
		oProtocol.iType = SOCK_STREAM;
		oProtocol.iProtocol = IPPROTO_TCP;
		break;
	case ESocketProtocol::SPUDP:
		oProtocol.iType = SOCK_DGRAM;
		oProtocol.iProtocol = IPPROTO_UDP;
		break;
	}

	return oProtocol;
}


Network::EError Network::CSocket::create(const ESocketProtocol i_eProtocol)
{
	this->close();

	this->m_oProtocolData = Network::CSocket::getProtocolData(i_eProtocol);
	if (!this->m_oProtocolData.isGood())
	{
		return Network::EError::EProtocolNotFound;
	}

	this->m_nSocket = WSASocketW(AF_INET, this->m_oProtocolData.iType, this->m_oProtocolData.iProtocol, nullptr, 0, WSA_FLAG_OVERLAPPED);//::socket(AF_INET, oProtocol.iType, oProtocol.iProtocol);
	if (!this->isValid()) {	
		return Network::EError::EWSALastError;
	}

	// NonBlocking = Iocp, Blocking is default so I don't need this anymore
	//u_long iValue = static_cast<u_long>(this->m_fIsNonBlocking);
	//ioctlsocket(this->m_nSocket, FIONBIO, &iValue);

	return Network::EError::ESuccess;
}

Network::EError Network::CSocket::bind()
{
	if (!this->isValid()) {
		return Network::EError::ENoSocket;
	} else if (::bind(this->m_nSocket, reinterpret_cast<const sockaddr *>(&this->address()), sizeof(this->address())) != 0) {
		return Network::EError::EWSALastError;
	}

	if (this->port() == 0)
	{
		SPeer oPeer; int iLength = sizeof(oPeer);
		getsockname(this->m_nSocket, reinterpret_cast<sockaddr *>(&oPeer), &iLength);

		this->setAddress(oPeer);
	}

	return Network::EError::ESuccess;
}


void Network::CSocket::close()
{
	if (this->m_nSocket != INVALID_SOCKET) {
		shutdown(this->m_nSocket, SD_BOTH);
		closesocket(this->m_nSocket);
	}

	this->m_nSocket = INVALID_SOCKET;
}


void *Network::CSocket::retrieveFunction(_GUID i_oGuid) const
{
	if (this->isValid())
	{
		DWORD dwBytesReturned;
		Function fnAddress;

		if (WSAIoctl(
			this->socket(), SIO_GET_EXTENSION_FUNCTION_POINTER,
			&i_oGuid, sizeof(i_oGuid),
			&fnAddress, sizeof(fnAddress),
			&dwBytesReturned, nullptr, nullptr) != SOCKET_ERROR)
		{
			return fnAddress;
		}
	}

	return nullptr;
}


void Network::CSocket::clear()
{
	this->close();
	this->clearAddress();
}
