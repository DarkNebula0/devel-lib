#include "SimpleWebRequest.h"
#include "Windows.h"

#define RECIVE_TIME_OUT 1500

Network::Web::CSimpleRequest::CSimpleRequest()
	: m_bFin(false)
{}



// Execute=================================================
std::string Network::Web::CSimpleRequest::simpleExecute()
{
	this->m_bFin = false;
	this->m_stBuffer.clear();
	this->m_oTimer.clear();

	if (CWebSocket::execute() == ERequestError::ESuccess)
	{
		while (!m_bFin)
		{
			Sleep(15);
			if (CWebSocket::isConnected())
			{
				if (!this->m_oTimer.isStarted())
				{
					this->m_oTimer.start();
				}
				if (this->m_oTimer.hasExpired(RECIVE_TIME_OUT))
				{
					break;
				}
			}
		}
	}

	return this->m_stBuffer;
}

void Network::Web::CSimpleRequest::onMessageReceived(const Network::CPacket & i_oPacket)
{
	this->m_stBuffer = std::string(i_oPacket.buffer(), i_oPacket.size());
	this->m_bFin = true;
}

std::string Network::Web::CSimpleRequest::httpRequest( const std::string &i_stUrl, const EMethods i_eMethod, const std::map<std::string, std::string> &i_aoHeaders, const std::string &i_stContent )
{
	CSimpleRequest oSimpleRequest;
	oSimpleRequest.setUrl(i_stUrl);

	if (oSimpleRequest.url()->protocol() != EProtocol::EWs && oSimpleRequest.url()->protocol() != EProtocol::EWss)
	{
		oSimpleRequest.setMethod(i_eMethod);
		CWebRequest oRequest;
		for (auto &oItem : i_aoHeaders)
		{
			oRequest.setHeader(oItem.first, oItem.second);
		}

		oRequest.setContent(i_stContent);

		oSimpleRequest.setRequest(oRequest);
	}
	return oSimpleRequest.simpleExecute();
}