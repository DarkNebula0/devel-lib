#pragma once

#include "Typedef.h"
#include "TCPClient.h"
#include "NetworkPeer.h"

#include <openssl/conf.h>
#include <functional>

namespace Network
{
	namespace Web
	{
		class CSecureClient : public CThread
		{
		public:
			CSecureClient();
			CSecureClient(const std::string &i_stIP, const ushort i_shPort);
			CSecureClient(const CPeer &i_oEndpoint);
			virtual ~CSecureClient();
		private:
			CPeer m_oEndpoint;
		private:
			SSL_CTX *m_pCTX;
			SSL		*m_pSSL;
		private:
			CTCPClient m_oClient;
		private:
			bool m_bHasSSLError;
		private:
			void handle(CPacket &i_oPacket) override;
		private:
			std::function<void(const Network::CPacket &i_oPacket)> m_oRecv;
			std::function<void()> m_oDisconnect;
		public:
			EError connect(const ushort i_shSecondsTimeout = 10);
			int send(const char *i_pBuffer, const size_t i_nSize) const;
		public:
			virtual void onPacketReceived(const Network::CPacket &i_oPacket);
			virtual void onDisconnected();
		public:
			_inline bool hasSSLError() const {return this->m_bHasSSLError;}
		public:
			_inline EError connect(const std::string &i_stIP, const ushort i_shPort, const ushort i_shSecondsTimeout = 10)
			{
				this->m_oEndpoint = CPeer(i_stIP, i_shPort);
				return this->connect(i_shSecondsTimeout);
			}
			_inline EError connect(const CPeer &i_oEndpoint, const ushort i_shSecondsTimeout = 10)
			{
				this->m_oEndpoint = i_oEndpoint;
				return this->connect(i_shSecondsTimeout);
			}
			_inline void setRecvCallback(const std::function<void(const Network::CPacket &i_oPacket)> &i_oFunction)
			{
				this->m_oRecv = i_oFunction;
			}
			_inline void setDisconnectCallback(const std::function<void()> &i_oFunction){ this->m_oDisconnect = i_oFunction;}
			_inline void closeConnection() { this->m_oClient.close(); }
		};
	}
}

typedef Network::Web::CSecureClient CSecureWebClient;