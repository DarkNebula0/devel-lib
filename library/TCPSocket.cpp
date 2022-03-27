#include "TCPSocket.h"
#include "TCPTask.h"

#include <WS2tcpip.h>


bool Network::TCP::CSocket::send(const char *i_pPacket, const uint i_iSize, const ESendFlag i_eFlag) const
{
	if (this->isNotBlocked())
	{	
		return this->startSend(i_pPacket, i_iSize, i_eFlag);
	}

	// Blocked
	uint iSent = 0;
	if (this->isValid() && i_pPacket)
	{
		while (iSent < i_iSize)
		{
			const uint iBytesSent = ::send(this->socket(), i_pPacket + iSent, i_iSize - iSent, static_cast<int>(i_eFlag));
			if (iBytesSent == SOCKET_ERROR)
			{
				break;
			}

			iSent += iBytesSent;
		}
	}

	return (iSent == i_iSize);
}

uint Network::TCP::CSocket::recv(char *i_pPacket, const uint i_iMaxSize, const ERecvFlag i_eFlag) const
{
	uint iRecv = (~0);
	if (this->isValid()) {
		iRecv = ::recv(this->socket(), i_pPacket, i_iMaxSize, static_cast<int>(i_eFlag));

		if (iRecv == 0) {
			iRecv = (~0);
		}
	}
	
	return iRecv;
}


bool Network::TCP::CSocket::startSend(const char *i_pPacket, const uint i_iSize, const ESendFlag i_eFlag) const
{
	Network::TCP::SSendTask *pTask = static_cast<Network::TCP::SSendTask *>(Network::TCP::SSendTask::operator new(sizeof(Network::TCP::SRecvTask), i_iSize));

	pTask->iFlags = static_cast<DWORD>(i_eFlag);
	pTask->nSocket = this->socket();

	memcpy(pTask->oUseBuf.oBuf.buf, i_pPacket, static_cast<size_t>(i_iSize));

	if (WSASend(this->socket(), &pTask->oUseBuf.oBuf, 1, &pTask->iBytesSent, pTask->iFlags, reinterpret_cast<LPWSAOVERLAPPED>(pTask), nullptr) == SOCKET_ERROR &&
		WSAGetLastError() != WSA_IO_PENDING)
	{
		delete pTask;
		return false;
	}

	return true;
}

void Network::TCP::CSocket::startRecv(const uint i_iReceiveBufferSize, const ERecvFlag i_eFlag) const
{
	Network::TCP::SRecvTask *pTask = static_cast<Network::TCP::SRecvTask *>(Network::TCP::SRecvTask::operator new(sizeof(Network::TCP::SRecvTask), i_iReceiveBufferSize));
	pTask->iFlags = static_cast<DWORD>(i_eFlag);
	pTask->nSocket = this->socket();

	if (WSARecv(this->socket(), &pTask->oBuf, 1, &pTask->iBytesReceived, &pTask->iFlags, reinterpret_cast<LPWSAOVERLAPPED>(pTask), nullptr) == SOCKET_ERROR &&
		WSAGetLastError() != WSA_IO_PENDING)
	{
		delete pTask;
		return;
	}
}