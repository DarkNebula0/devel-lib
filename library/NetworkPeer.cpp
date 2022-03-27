#include "NetworkPeer.h"
#include "Global.h"

#include <WS2tcpip.h>


Network::EError Network::CPeer::setAddress(const std::string &i_sIp, const ushort i_wPort)
{
	this->clearAddress();

	SPeer &oAddress     = reinterpret_cast<SPeer &>(this->m_abAddress);
	oAddress.sin_family = AF_INET;
	oAddress.sin_port	= (i_wPort > 0 ? htons(i_wPort) : i_wPort);

	const int iError = inet_pton(oAddress.sin_family, i_sIp.c_str(), &oAddress.sin_addr.s_addr);
	if (iError != 1)
	{
		return (iError == 0 ? EIPFormat : EWSALastError);
	}

	return ESuccess;
}


std::string Network::CPeer::ip() const
{
	char szIp[INET_ADDRSTRLEN];
	inet_ntop(this->address().sin_family, &this->address().sin_addr.s_addr, szIp, INET_ADDRSTRLEN);

	return std::string(szIp);
}

ushort Network::CPeer::port() const
{
	return htons(this->address().sin_port);
}


void Network::CPeer::clearAddress()
{
	ZeroMemory(this->m_abAddress, sizeof(this->m_abAddress));
}
