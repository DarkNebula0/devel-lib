#include "NetworkThread.h"
#include "NetworkPacket.h"


void Network::CThread::run()
{
	this->m_bIsRunning = true;

	CPacket oPacket(this->m_nMaxReceiveBytes * this->m_nPacketHandlerCount);
	this->handle(oPacket);

	this->m_bIsExecuted = false;
	this->m_bIsRunning = false;
}

bool Network::CThread::execute(const size_t i_nMaxReceiveBytes)
{
	if (!this->m_bIsExecuted && !this->m_bIsRunning && i_nMaxReceiveBytes != 0)
	{
		this->stop(true);

		this->m_nMaxReceiveBytes = i_nMaxReceiveBytes;
		this->m_bIsExecuted = true;

		this->m_oThread = std::thread(&Network::CThread::run, this);

		return true;
	}

	return false;
}