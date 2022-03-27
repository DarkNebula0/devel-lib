#pragma once
#include "NetworkSocket.h"
#include "NetworkPacket.h"
#include "NetworkThread.h"
#include "Typedef.h"

#include <string>

namespace Network
{
	namespace UDP
	{
		class CSocket : public Network::CSocket, public CThread
		{
		public:
			CSocket()
				: m_fHasError(false)
			{
				if (Network::CSocket::create(ESocketProtocol::SPUDP) != Network::EError::ESuccess)
				{
					this->m_fHasError = true;
				}
			}

			CSocket(const std::string &i_sIp, const ushort i_shPort)
				: CSocket()
			{
				if (!this->m_fHasError && Network::CSocket::setAddress(i_sIp, i_shPort) != Network::EError::ESuccess)
				{
					this->m_fHasError = true;
				}
			}

			~CSocket() = default;

		public:
			void handle(CPacket &i_oPacket) override;

			void sendTo(const sockaddr_in &i_oAddr, const char *i_pBuffer, uint i_nSize) const;
			void sendTo(const sockaddr_in &i_oAddr, const CPacket &i_oPacket) const
			{
				return this->sendTo(i_oAddr, i_oPacket.buffer(), static_cast<uint>(i_oPacket.size()));
			}

		public:
			virtual void onPacketReceived(const CPeer &i_oPeer, const CPacket &i_oPacket) {}

		private:
			bool m_fHasError;
		};
	};
};

typedef Network::UDP::CSocket CUDPSocket;
