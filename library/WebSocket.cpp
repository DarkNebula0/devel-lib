#include "WebSocket.h"
#include "WriteStream.h"
#include "Base64.h"
#include "Logger.h"

#include <WS2tcpip.h>

#define DEFAULT_STRING_BUFFER_SIZE 250

Network::Web::CWebSocket::CWebSocket( const CUrl &i_oUrl, const EMethods i_eMethod, const ushort i_shSecondsTimeout )
	: m_eMethod(i_eMethod), m_shTimeout(i_shSecondsTimeout), m_nMaxReceiveBytes(2048), m_bIsConnected(false)
	, m_bIsWsSwitchingComplete(false), m_oUrl(i_oUrl), m_pSecureClient(nullptr), m_pClient(nullptr)
	, m_pWriteStream(nullptr), m_pFrameBuffer(new CWebFrameBuffer)
{}

Network::Web::CWebSocket::~CWebSocket()
{
	delete this->m_pClient;
	delete this->m_pSecureClient;
	delete this->m_pWriteStream;
	delete this->m_pFrameBuffer;
}

// Connect=================================================
Network::Web::ERequestError Network::Web::CWebSocket::connect()
{
	ERequestError eError = ERequestError::ESuccess;

	// Get host id
	SPeer const &grHost = this->m_oUrl.getHostData();

	if (grHost.sin_port != 0x00 && grHost.sin_addr.S_un.S_addr != 0x00)
	{
		EError eConnection = EError::ESuccess;

		if (this->m_bIsConnected)
		{
			eError = ERequestError::EAlreadyConnected;
		}
		else
		{
			// Switch sockets
			if (this->m_oUrl.isSecure())
			{
				// Secure client
				if (!this->m_pSecureClient)
				{
					this->m_pSecureClient = new CSecureClient();
					this->m_pSecureClient->setDisconnectCallback(std::bind(&CWebSocket::onDisconnect, this));
					this->m_pSecureClient->setRecvCallback(std::bind(&CWebSocket::onPacketReceived, this, std::placeholders::_1));
				}

				eConnection = this->m_pSecureClient->connect(grHost, this->m_shTimeout);
			}
			else
			{
				if (!this->m_pClient)
				{
					this->m_pClient = new CTCPClient();
					this->m_pClient->setDisconnectCallback(std::bind(&CWebSocket::onDisconnect, this));
					this->m_pClient->setRecvCallback(std::bind(&CWebSocket::onPacketReceived, this, std::placeholders::_1));
				}

				eConnection = this->m_pClient->connect(grHost, this->m_shTimeout);
			}

			if (eConnection == EError::ESuccess)
			{
				this->m_bIsConnected = true;
				if (this->m_oUrl.isSecure())
				{
					this->m_pSecureClient->execute(static_cast< uint >(this->m_nMaxReceiveBytes));
				}
				else
				{
					// ToDo: TCPClient rewrite
					//this->m_pClient->execute(static_cast< uint >(this->m_nMaxReceiveBytes));
				}
			}
			else if (eConnection == EError::ETimeout)
			{
				eError = ERequestError::ETimeout;
			}
			else
			{
				eError = ERequestError::EInvalidHost;
			}
		}
	}
	else
	{
		eError = ERequestError::EInvalidHost;
	}

	return eError;
}

// Execute http request====================================
Network::Web::ERequestError Network::Web::CWebSocket::executeHttpRequest()
{
	ERequestError eError = ERequestError::ESuccess;
	std::string stBuffer;
	stBuffer.reserve(DEFAULT_STRING_BUFFER_SIZE);

	// Set methode
	switch (this->m_eMethod)
	{
	case EMethods::EGet: stBuffer += "GET ";
		break;
	case EMethods::EHead: stBuffer += "HEAD ";
		break;
	case EMethods::EPost: stBuffer += "POST ";
		break;
	case EMethods::EPut: stBuffer += "PUT ";
		break;
	case EMethods::EDelete: stBuffer += "DELETE ";
		break;
	case EMethods::EConnect: stBuffer += "CONNECT ";
		break;
	case EMethods::EOptions: stBuffer += "OPTIONS ";
		break;
	case EMethods::EPatch: stBuffer += "PATCH ";
		break;
	default:
		eError = ERequestError::EInvalidMethode;
	}

	if (eError == ERequestError::ESuccess)
	{
		stBuffer += this->m_oUrl.resource();

		// Set header version TODO: evtl. changeable
		stBuffer += " HTTP/1.1\r\n";

		// Set header
		{
			// Pre header (Host, Connection, Content-Length, Authorization)
			{
				if (this->m_oRequest.m_astHeader.find("Host") == this->m_oRequest.m_astHeader.end())
				{
					this->m_oRequest.m_astHeader["Host"] = this->m_oUrl.host();
				}

				if (this->m_oRequest.m_astHeader.find("Connection") == this->m_oRequest.m_astHeader.end())
				{
					this->m_oRequest.m_astHeader["Connection"] = "close";
				}

				if (this->m_oRequest.m_astHeader.find("Content-Length") == this->m_oRequest.m_astHeader.end())
				{
					this->m_oRequest.m_astHeader["Content-Length"] = std::to_string(this->m_oRequest.contentSize());
				}

				if (!this->m_oUrl.authorization().empty())
				{
					if (this->m_oRequest.m_astHeader.find("Authorization") == this->m_oRequest.m_astHeader.end())
					{
						Base64 oHash(this->m_oUrl.authorization());
						this->m_oRequest.m_astHeader["Authorization"] = "Basic " + oHash.encode();
					}
				}
			}

			for (auto &it : this->m_oRequest.m_astHeader)
			{
				stBuffer += it.first + ": " + it.second + "\r\n";
			}

			stBuffer += "\r\n";
		}

		// Set content
		if (this->m_oRequest.hasContent())
		{
			stBuffer += this->m_oRequest.content();
			stBuffer += "\r\n";
		}

		if (this->m_bIsConnected)
		{
			int nResult = -1;
			if (this->m_oUrl.isSecure())
			{
				nResult = this->m_pSecureClient->send(stBuffer.c_str(), stBuffer.size());
			}
			else
			{
				// ToDo: Rewrite this because of new TCPClient
				//nResult = this->m_pClient->send(stBuffer.c_str(), static_cast< uint >(stBuffer.size()));
			}

			if (nResult < 0)
			{
				eError = ERequestError::ESocketError;
			}
		}
		else
		{
			eError = ERequestError::EConnectionLost;
		}
	}

	return eError;
}

// Create ws login request=================================
void Network::Web::CWebSocket::createWsLoginRequest()
{
	this->m_bIsWsSwitchingComplete = false;

	// Set websocket header
	this->m_oRequest.m_astHeader["Upgrade"] = "websocket";
	this->m_oRequest.m_astHeader["Connection"] = "Upgrade";
	this->m_oRequest.m_astHeader["Sec-WebSocket-Key"] = "dGhlIHNhbXBsZSBub25jZQ==";
	this->m_oRequest.m_astHeader["Sec-WebSocket-Version"] = "13";
}

// On disconnect===========================================
void Network::Web::CWebSocket::onDisconnect()
{
	if (this->m_pSecureClient)
	{
		this->m_pSecureClient->stop();
	}
	if (this->m_pClient)
	{
		this->m_pClient->stop();
	}

	this->m_bIsConnected = false;

	// Call on packet recive (on http connect type close)
	if (this->m_pWriteStream)
	{
		if (this->m_pWriteStream->size() > 0)
		{
			return this->onMessageReceived(CPacket(this->m_pWriteStream->buffer(), static_cast<uint>(this->m_pWriteStream->size()), true, true));
		}
	}

	return this->onDisconnected();
}

// On packet received======================================
void Network::Web::CWebSocket::onPacketReceived( const Network::CPacket &i_oPacket )
{
	if (this->m_oUrl.protocol() == EProtocol::EWs || this->m_oUrl.protocol() == EProtocol::EWss)
	{
		this->onHandleWs(i_oPacket);
	}
	else
	{
		this->onHandleHttp(i_oPacket);
	}
}

// On handle ws | wss response=============================
void Network::Web::CWebSocket::onHandleWs(const Network::CPacket &i_oPacket)
{
	if (!this->m_bIsWsSwitchingComplete)
	{
		std::string stBuffer(i_oPacket.buffer(), i_oPacket.size());
		if (stBuffer.find("Switching Protocols") != std::string::npos ||
			stBuffer.find("101") != std::string::npos)
		{
			this->m_bIsWsSwitchingComplete = true;
		}
		else
		{
			this->closeConnection();
		}
	}
	else
	{	
		const char *pBuffer = i_oPacket.buffer();
		size_t nSize = i_oPacket.size();

		while (true)
		{
			const int nCode = this->m_pFrameBuffer->readFrames(CNetworkPacket(pBuffer, static_cast<uint>(nSize), false, false));

			if (nCode < 0)
			{
				return this->closeConnection();
			}
			else
			{
				if (nCode == 1)
				{
					this->onMessageReceived(this->m_pFrameBuffer->getPacket());
					return;
				}
				else if (nCode > 3)
				{
					pBuffer = i_oPacket.buffer() + (i_oPacket.size() - nCode);
					nSize = nCode;
				}
				else
				{
					return;
				}
			}
		}
	}
}

// On handle http | https response=========================
void Network::Web::CWebSocket::onHandleHttp(const Network::CPacket &i_oPacket)
{
	if (!this->m_pWriteStream)
		return;

	if (this->m_oRequest.getHeader("Connection") == "close")
	{
		this->m_pWriteStream->push(i_oPacket.buffer(), i_oPacket.size());
	}
	else
	{
		// TODO: header legth and ......
		LOGE("Wenn dieser fehler kommt sag bescheid");
	}
}

// On message received=====================================
void Network::Web::CWebSocket::onMessageReceived( const Network::CPacket &i_oPacket )
{
	if (this->m_oOnMessageRecv)
	{
		return this->m_oOnMessageRecv(i_oPacket);
	}
}

// On disconnected=========================================
void Network::Web::CWebSocket::onDisconnected()
{
	if (this->m_oOnDisconnected)
	{
		return this->m_oOnDisconnected();
	}
}

// Execute=================================================
Network::Web::ERequestError Network::Web::CWebSocket::execute()
{
	ERequestError eError = this->connect();

	if (eError == ERequestError::ESuccess)
	{
		if (!this->m_pWriteStream)
		{
			this->m_pWriteStream = new CWriteStream(this->m_nMaxReceiveBytes);
		}
		else
		{
			this->m_pWriteStream->clear();
			this->m_pWriteStream->reallocate(this->m_nMaxReceiveBytes);
		}
		
		if (this->m_oUrl.protocol() == EProtocol::EWs || this->m_oUrl.protocol() == EProtocol::EWss)
		{
			createWsLoginRequest();
		}

		eError = executeHttpRequest();
	}

	return eError;
}

// Execute frame buffer====================================
Network::Web::ERequestError Network::Web::CWebSocket::execute(const CWebFrameBuffer &i_oBuffer) const
{
	ERequestError eError = ERequestError::ESocketError;

	if (this->isConnected())
	{
		CWriteStream oStream = i_oBuffer.createFrame();

		if (this->m_oUrl.isSecure())
		{
			if(this->m_pSecureClient)
			{
				if (this->m_pSecureClient->send(oStream.buffer(), oStream.size()) > 0)
				{
					eError = ERequestError::ESuccess;
				}
			}
	
		}
		else if (this->m_pClient)
		{
			// ToDo: Rewrite this because of new TCPClient
			/*if (this->m_pClient->send(oStream.buffer(), static_cast<uint>(oStream.size())))
			{
				eError = ERequestError::ESuccess;
			}*/
		}
	}
	return eError;
}

// Close connection========================================
void Network::Web::CWebSocket::closeConnection() const
{
	if (this->m_pSecureClient)
		this->m_pSecureClient->closeConnection();
	if (this->m_pClient)
		this->m_pClient->close();
}