#pragma once
#include "Typedef.h"
#include "Global.h"

#include <string>

namespace Network
{
	class CPeer
	{
	public:
		CPeer()
		{
			this->clearAddress();
		}

		CPeer(const SPeer &i_oPeer)
			: CPeer()
		{
			this->setAddress(i_oPeer);
		}

		CPeer(const std::string &i_sIp, const ushort i_wPort)
			: CPeer()
		{
			this->setAddress(i_sIp, i_wPort);
		}

		virtual ~CPeer() = default;

	public:
		EError setAddress(const Network::SEndpoint &i_oEndpoint)
		{
			return this->setAddress(i_oEndpoint.sIp, i_oEndpoint.wPort);
		}
		EError setAddress(const std::string &i_sIp, ushort i_wPort);
		void setAddress(const SPeer &i_grPeer)
		{
			memcpy(this->m_abAddress, &i_grPeer, sizeof(this->m_abAddress));
		}

	public:
		std::string ip() const;
		ushort port() const;

	public:
		const SPeer &address() const
		{
			return reinterpret_cast<const SPeer &>(this->m_abAddress);
		}

	public:
		void operator =(const CPeer &i_oPeer)
		{
			return this->operator=(&i_oPeer);
		}

		void operator =(const CPeer *i_pPeer)
		{
			return this->setAddress(i_pPeer->address());
		}

	protected:
		void clearAddress();

	private:
		char m_abAddress[16];
	};
}
