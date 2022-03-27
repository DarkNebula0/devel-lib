#pragma once
#include <thread>
#include "Typedef.h"

namespace Network
{
	class CThread
	{
		friend class Network::TCP::CServer;
	public:
		CThread()
			: m_bIsExecuted(false), m_bIsRunning(false), m_nMaxReceiveBytes(0), m_nPacketHandlerCount(1)
		{
		}

		virtual ~CThread()
		{
			this->stop(true);
		}

	private:
		void run();

	public:
		bool execute(size_t i_nMaxReceiveBytes = 2048);
		
		void stop(const bool i_fJoin = false)
		{
			this->m_bIsExecuted = false;
			if (this->m_oThread.joinable())
			{
				i_fJoin ? this->m_oThread.join() : this->m_oThread.detach();
			}
		}

		bool isStopped() const
		{
			return !this->m_bIsRunning;
		}

		bool isRunning() const
		{
			return this->m_bIsExecuted;
		}

		virtual void handle(CPacket &i_oPacket) {}

	private:
		void setPacketHandlerCount(const size_t i_nPacketHandlerCount)
		{
			this->m_nPacketHandlerCount = (i_nPacketHandlerCount > 0 ? i_nPacketHandlerCount : 1);
		}

		size_t packetHandlerCount() const
		{
			return this->m_nPacketHandlerCount;
		}

		size_t maxReceiveBytes() const
		{
			return this->m_nMaxReceiveBytes;
		}

	private:
		bool m_bIsExecuted;
		bool m_bIsRunning;

		size_t m_nMaxReceiveBytes;
		size_t m_nPacketHandlerCount;

		std::thread m_oThread;
	};
};