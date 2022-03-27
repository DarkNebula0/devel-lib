#pragma once

#include "WebSocket.h"
#include "Timer.h"
#include "map"

namespace Network
{
	namespace Web
	{
		class CSimpleRequest : private CWebSocket
		{
		private:
			CSimpleRequest();
			virtual ~CSimpleRequest() = default;
		private:
			bool m_bFin;
		private:
			std::string m_stBuffer;
		private:
			CTimer m_oTimer;
		private:
			std::string simpleExecute();
		private:
			void onMessageReceived(const Network::CPacket &i_oPacket) override;
		public:
			static std::string httpRequest(const std::string &i_stUrl, const EMethods i_eMethod = EMethods::EGet, const std::map<std::string, std::string> &i_aoHeaders = {}, const std::string &i_stContent = "");
		};	
	}
}

typedef Network::Web::CSimpleRequest CSimpleWebRequest;