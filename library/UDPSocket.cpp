#include "UDPSocket.h"
#include "Global.h"

#include <WinSock2.h>
#include <thread>



void Network::UDP::CSocket::sendTo(const sockaddr_in &i_oAddr, const char *i_pBuffer, const uint i_nSize) const
{
	if (Network::CSocket::isValid() && !this->m_fHasError)
	{
		uint nSend = 0x00;

		while (nSend < i_nSize)
		{
			const uint iBytesSent = sendto(Network::CSocket::socket(), i_pBuffer + nSend, i_nSize - nSend, 0, reinterpret_cast<const SOCKADDR*>(&i_oAddr), sizeof(SOCKADDR_IN));
			if (iBytesSent == SOCKET_ERROR)
			{
				break;
			}

			nSend += iBytesSent;
		}
	}
}

void Network::UDP::CSocket::handle(CPacket &i_oPacket)
{
	if (!Network::CSocket::isValid())
		return;

	SPeer oRemotePeer;

	while (Network::CThread::isRunning())
	{
		THREAD_SLEEP();

		int nPeerSize = sizeof(oRemotePeer);

		const uint nSize = recvfrom(Network::CSocket::socket(), const_cast<char*>(i_oPacket.buffer()), static_cast<int>(i_oPacket.size()), 0, reinterpret_cast<SOCKADDR*>(&oRemotePeer), &nPeerSize);
		if (nSize != (~0))
		{
			this->onPacketReceived(CPeer(oRemotePeer), CPacket(i_oPacket.buffer(), nSize, false));
		}
	}
}