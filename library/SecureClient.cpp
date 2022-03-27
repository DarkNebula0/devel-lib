#include "SecureClient.h"
#include "Global.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <thread>
#include <WinSock2.h>

Network::Web::CSecureClient::CSecureClient()
	: m_pCTX(nullptr), m_pSSL(nullptr), m_bHasSSLError(false)
{
	// Set blocking type
	// ToDo: TCPClient fix
	//this->m_oClient.setBlockingType(false);

	// Init ssl
	SSL_library_init();
	OpenSSL_add_all_algorithms();

	SSL_load_error_strings();
    ERR_load_crypto_strings();

	this->m_pCTX = SSL_CTX_new(SSLv23_client_method());
	if (!this->m_pCTX)
	{
		this->m_bHasSSLError = true;
		return;
	}

	SSL_CTX_set_options(this->m_pCTX, SSL_OP_NO_SSLv2);

	// Create ssl
	this->m_pSSL = SSL_new(this->m_pCTX);
	if (!this->m_pSSL)
	{
		this->m_bHasSSLError = true;
	}
}

Network::Web::CSecureClient::CSecureClient( const std::string &i_stIP, const ushort i_shPort )
	: CSecureClient()
{
	this->m_oEndpoint = CPeer(i_stIP, i_shPort);
}

Network::Web::CSecureClient::CSecureClient( const CPeer &i_oEndpoint )
	: CSecureClient()
{
	this->m_oEndpoint = i_oEndpoint;
}

Network::Web::CSecureClient::~CSecureClient()
{
	SSL_shutdown(this->m_pSSL);
	SSL_free(this->m_pSSL);
	SSL_CTX_free(this->m_pCTX);
}

// Handle==================================================
void Network::Web::CSecureClient::handle( CPacket &i_oPacket )
{
	if (!this->m_oClient.isValid() || this->hasSSLError())
	{
		return;
	}

	static timeval oTimeout = { 0, 0 };
	fd_set oSet;

	while (this->isRunning())
	{
		THREAD_SLEEP();

		FD_ZERO(&oSet);
		FD_SET(this->m_oClient.socket(), &oSet);
	
		if (select(0, &oSet, nullptr, nullptr, &oTimeout) != 0x00)
		{
			const int nRecv = SSL_read(this->m_pSSL, const_cast< char * >(i_oPacket.buffer()), static_cast<int>(i_oPacket.size()));
			if (nRecv > 0)
			{
				this->onPacketReceived(CPacket(i_oPacket.buffer(), nRecv));
			}
			else
			{
				return onDisconnected();
			}
		}
	}
}

Network::EError Network::Web::CSecureClient::connect( const ushort i_shSecondsTimeout )
{
	EError eError = EError::ENoSocket;
	if (!this->m_bHasSSLError)
	{
		eError = this->m_oClient.connect(this->m_oEndpoint.ip(), this->m_oEndpoint.port(), i_shSecondsTimeout);
		if (eError == EError::ESuccess)
		{
			if (SSL_set_fd(this->m_pSSL, static_cast<int>(this->m_oClient.socket())) != 0x01)
			{
				eError = EError::EWSALastError;
			}
			else
			{
				// Send ssl handshake
				SSL_connect(this->m_pSSL);
			}
		}
	}

	return eError;
}

int Network::Web::CSecureClient::send( const char *i_pBuffer, const size_t i_nSize ) const
{
	if (this->hasSSLError())
		return -1;
	
	return SSL_write(this->m_pSSL, i_pBuffer, static_cast< int >(i_nSize));
}

void Network::Web::CSecureClient::onPacketReceived(const Network::CPacket &i_oPacket)
{
	if (this->m_oRecv)
	{
		this->m_oRecv(i_oPacket);
	}
}

void Network::Web::CSecureClient::onDisconnected()
{
	if (this->m_oDisconnect)
	{
		return this->m_oDisconnect();
	}
}