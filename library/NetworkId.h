#pragma once
#include <type_traits>
#include <atomic>

#include "MutexVector.h"

namespace Network
{
	class IId
	{
	public:
		IId(const uint64 i_nId = ~0)
			: m_nId(i_nId)
		{ }

	public:
		void setNetworkId(const uint64 i_nId)
		{
			this->m_nId = i_nId;
		}

		uint64 networkId() const
		{
			return this->m_nId;
		}

	private:
		uint64 m_nId;
	};

	template<typename TSession, typename std::enable_if<std::is_base_of<Network::IId, TSession>::value>::type * = nullptr>
	class IIdHandler
	{
		typedef std::shared_ptr<TSession> Session;
	public:
		IIdHandler(CMutexVector<Session> &i_pSessions)
			: m_pSessions(i_pSessions), m_nLastId(0)
		{ }

	public:
		Session findNetworkId(const uint64 i_nNetworkId)
		{
			auto &apSessions = this->m_pSessions;
			
			MutexVectorLockGuard(apSessions);
			for (auto &pSession : apSessions)
			{
				if (pSession->networkId() == i_nNetworkId)
				{
					return pSession;
				}
			}

			return nullptr;
		}

	public:
		uint64 newNetworkId()
		{
			return this->m_nLastId++;
		}

	private:
		CMutexVector<Session> &m_pSessions;
		std::atomic<uint64> m_nLastId;
	};
}