#pragma once

#include "WebFrameBuffer.h"
#include "SecureClient.h"
#include "WebRequest.h"
#include "UrlParser.h"
#include "TCPClient.h"
#include "Typedef.h"
#include "Global.h"

#include <functional>
#include <string>

class CWriteStream;
namespace Network
{
	namespace Web
	{	
		class CWebSocket
		{
		public:
			CWebSocket() : CWebSocket(CUrl(), EMethods::EGet, 0x0A) {};
			explicit CWebSocket(const CUrl &i_oUrl, const EMethods i_eMethod = EMethods::EGet, const ushort i_shSecondsTimeout = 0x0A);
			virtual ~CWebSocket();
		private:
			EMethods m_eMethod;
		private:
			ushort m_shTimeout;
			size_t m_nMaxReceiveBytes;
		private:
			bool m_bIsConnected;
			bool m_bIsWsSwitchingComplete;
		private:
			CUrl m_oUrl;
			CWebRequest m_oRequest;
		private:
			CSecureClient *m_pSecureClient;
			CTCPClient *m_pClient;
			CWriteStream *m_pWriteStream;
			CWebFrameBuffer *m_pFrameBuffer;
		private:
			std::function<void(const Network::CPacket &i_oPacket)> m_oOnMessageRecv;
			std::function<void()> m_oOnDisconnected;
		private:
			ERequestError connect();
			ERequestError executeHttpRequest();
		private:
			void createWsLoginRequest();
		private:
			void onDisconnect();
			void onPacketReceived(const Network::CPacket &i_oPacket);
			void onHandleWs(const Network::CPacket &i_oPacket);
			void onHandleHttp(const Network::CPacket &i_oPacket);
		public:
			virtual void onMessageReceived(const Network::CPacket &i_oPacket);
			virtual void onDisconnected();
		public:
			ERequestError execute();
			ERequestError execute(const CWebFrameBuffer &i_oBuffer) const;
			void closeConnection() const;		
		public:
			_inline ushort timeoutInSeconds() const { return this->m_shTimeout; }
			_inline size_t maxReceiveBytes() const { return this->m_nMaxReceiveBytes; }
			_inline EMethods methode() const { return this->m_eMethod; }
			_inline bool isConnected() const {return this->m_bIsConnected;}
			_inline const CUrl *url() const { return &this->m_oUrl; }
			_inline const CWebRequest *request() const { return &this->m_oRequest; }
		public:
			inline ERequestError execute(const CWebRequest &i_oRequest)
			{
				this->m_oRequest = i_oRequest;
				return this->execute();
			}
		public:
			_inline bool setUrl(const std::string &i_stUrl) { return this->m_oUrl.setUrl(i_stUrl); }
			_inline void setMethod(const EMethods i_eMethod) { this->m_eMethod = i_eMethod; }
			_inline void setRequest(const CWebRequest &i_oRequest) { this->m_oRequest = i_oRequest; }
			_inline void setMaxReceiveBytes(const size_t i_nMaxReceiveBytes) { this->m_nMaxReceiveBytes = i_nMaxReceiveBytes; }
			_inline void setOnMessageCallback(const std::function<void(const Network::CPacket &i_oPacket)> &i_oFunction) { this->m_oOnMessageRecv = i_oFunction; }
			_inline void setOnDisconnectedCallback(const std::function<void()> &i_oFunction) { this->m_oOnDisconnected = i_oFunction; }
		};
	}
}