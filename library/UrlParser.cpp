#include "UrlParser.h"
#include "NetworkSocket.h"
#include "WsaGlobal.h"

#include <WS2tcpip.h>
#include <iterator>

// Define ports
#define DEFAULT_PORT 80
#define DEFAULT_SECURE_PORT 443

// Clear===================================================
void Network::Web::CUrl::clear()
{	
	memset(this->m_achHostData, 0, 16);

	this->m_eProtocol = EProtocol::ENothing;
	this->m_shPort = 0;
	this->m_bIsValid = false;
	this->m_bIsSecure = false;
	this->m_bHasValidHostData = false;

	this->m_stUrl.clear();
	this->m_stHost.clear();
	this->m_stResource.clear();
	this->m_stQuery.clear();
	this->m_stAuthorization.clear();
}

// Extract=================================================
void Network::Web::CUrl::extract()
{
	const size_t nSize = this->m_stUrl.size();
	std::string::const_iterator oIterator = this->m_stUrl.cbegin();

	// Get protocol
	{
		struct SProtocol
		{
			bool bIsSecure;
			EProtocol eProtocol;
			std::string stProtocol;
		};

		static SProtocol agrProtocols[] =
		{
			{ true, EProtocol::EHttps, "https://" },
			{ false,EProtocol::EHttp, "http://" },
			{ true, EProtocol::EWss, "wss://"},
			{ false, EProtocol::EWs,  "ws://"},
		};

		for (SProtocol &grItem : agrProtocols)
		{
			if (nSize >= grItem.stProtocol.size() + 1 &&
				std::equal(oIterator, oIterator + grItem.stProtocol.size(), grItem.stProtocol.begin()))
			{
				this->m_bIsSecure = grItem.bIsSecure;
				this->m_eProtocol = grItem.eProtocol;
				oIterator += grItem.stProtocol.size();
				break;
			}

			if (&grItem == &agrProtocols[SIZEOF_ARRAY(agrProtocols) - 1])
			{
				return;
			}
		}
	}
	
	// Serch for credentials
	{
		std::string stTemp(oIterator, std::string::const_iterator(this->m_stUrl.end()));
		const size_t nPos = stTemp.find('@');
		if (nPos != std::string::npos && nPos < stTemp.find('?') && nPos < stTemp.find('/'))
		{
			this->m_stAuthorization = stTemp.substr(0, nPos);
			oIterator += nPos + 1;
		}
	}

	// Extract host
	byte chState = 0x02;
	{
		if (*oIterator == '[')
		{
			// IPV6
			std::string::const_iterator tIterator = ++oIterator;
			while (tIterator != this->m_stUrl.end() || *tIterator == ']') { ++tIterator; }

			// Check iterator
			if (tIterator == this->m_stUrl.end())
			{

				return;
			}
		
			this->m_stHost.append(oIterator, tIterator);
			
			oIterator = ++tIterator;

			if (oIterator != this->m_stUrl.end())
			{
				if (*oIterator == ':')
				{
					chState = 0x01;
				}

				++oIterator;
			}
		}
		else
		{
			// IPV4
			while (true)
			{
				if (oIterator == this->m_stUrl.end() || *oIterator == '/')
				{
					break;
				}
				else if (*oIterator == ':')
				{
					chState = 0x01;
					++oIterator;
					break;
				}
				else
				{
					this->m_stHost += *oIterator;
				}

				++oIterator;
			}
		}
	}

	// Extract prot
	{
		std::string stPort;
		while (chState == 0x01)
		{
			if (oIterator != this->m_stUrl.end())
			{
				if (*oIterator == '/')
				{
					chState = 0x03;
					break;
				}
				else
				{
					stPort += *oIterator;
					++oIterator;
				}
			}	
			else
			{
				break;
			}
		}

		if (stPort.empty())
		{
			if (this->m_bIsSecure)
			{
				this->m_shPort = DEFAULT_SECURE_PORT;
			}
			else this->m_shPort = DEFAULT_PORT;
		}
		else
		{
			this->m_shPort = static_cast<ushort>(Utils::StringToInt(stPort));
			if (this->m_shPort == 0x00)
				return;
		}

	}
	
	this->m_stResource = "/";
	if (oIterator != this->m_stUrl.end())
	{
		++oIterator;
		this->m_stResource.append(oIterator, std::string::const_iterator(this->m_stUrl.end()));

		// Check query
		{
			size_t nPos = this->m_stResource.find('?');
			if (nPos != std::string::npos)
			{
				this->m_stQuery = this->m_stResource.substr(nPos++);
			}
		}
	}

	this->m_bIsValid = true;
}

Network::Web::CUrl &Network::Web::CUrl::operator=(const CUrl *i_pOther)
{
	if (!i_pOther)
	{
		this->clear();
	}
	else
	{
		memcpy(this->m_achHostData, i_pOther->m_achHostData, 16);
		this->m_eProtocol			= i_pOther->m_eProtocol;
		this->m_shPort				= i_pOther->m_shPort;
		this->m_bIsValid			= i_pOther->m_bIsValid;
		this->m_bIsSecure			= i_pOther->m_bIsSecure;
		this->m_bHasValidHostData	= i_pOther->m_bHasValidHostData;
		this->m_stUrl				= i_pOther->m_stUrl;
		this->m_stHost				= i_pOther->m_stHost;
		this->m_stResource			= i_pOther->m_stResource;
		this->m_stQuery				= i_pOther->m_stQuery;
		this->m_stAuthorization		= i_pOther->m_stAuthorization;
	}
	return *this;
}
Network::Web::CUrl &Network::Web::CUrl::operator=(CUrl &&i_oOther)
{	
	this->m_eProtocol			= i_oOther.m_eProtocol;
	this->m_shPort				= i_oOther.m_shPort;
	this->m_bIsValid			= i_oOther.m_bIsValid;
	this->m_bIsSecure			= i_oOther.m_bIsSecure;
	this->m_bHasValidHostData	= i_oOther.m_bHasValidHostData;

	this->m_stUrl				= std::move(i_oOther.m_stUrl);
	this->m_stHost				= std::move(i_oOther.m_stHost);
	this->m_stResource			= std::move(i_oOther.m_stResource);
	this->m_stQuery				= std::move(i_oOther.m_stQuery);
	this->m_stAuthorization		= std::move(i_oOther.m_stAuthorization);
	
	memcpy(this->m_achHostData,	i_oOther.m_achHostData, 16);
	i_oOther.clear();

	return *this;
}

// Get host data===========================================
const sockaddr_in &Network::Web::CUrl::getHostData()
{
	sockaddr_in &grData = reinterpret_cast<sockaddr_in&>(this->m_achHostData);

	if (!this->m_bHasValidHostData)
	{
		if (this->m_bIsValid)
		{
			addrinfo *pInfo;
			InitializeWSA();

			if (!getaddrinfo(this->m_stHost.data(), nullptr, nullptr, &pInfo))
			{
				this->m_bHasValidHostData = true;
				grData = *reinterpret_cast<sockaddr_in*>(pInfo->ai_addr);
				grData.sin_port = htons(this->m_shPort);
				return grData;
			}
		}
		memset(&grData, 0, sizeof(sockaddr_in));
	}
	return grData;
}