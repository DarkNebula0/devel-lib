#pragma once
#include <string>

#include "NetworkPeer.h"
#include "Typedef.h"
#include "Global.h"

namespace Network {
	class CSocket : public Network::CPeer
	{
	public:
		CSocket();
		CSocket(CSocket *i_pSocket) : CSocket() { this->operator=(i_pSocket); }
		CSocket(CSocket &i_oSocket) : CSocket() { this->operator=(i_oSocket); }
		CSocket(const std::string &i_sIp, const ushort i_wPort) : CSocket() { this->setAddress(i_sIp, i_wPort); }
		virtual ~CSocket() { this->clear(); }

	public:
		static SProtocolData getProtocolData(ESocketProtocol i_eProtocol);

	public:
		EError create(ESocketProtocol i_eProtocol);
		EError create(const bool i_fIsNonBlocking, const ESocketProtocol i_eProtocol)
		{
			this->setIsNonBlocking(i_fIsNonBlocking);
			return this->create(i_eProtocol);
		}

		EError bind();
		EError bind(const std::string &i_sIp, const ushort i_wPort) {
			const Network::EError eError = this->setAddress(i_sIp, i_wPort);
			if (eError != Network::EError::ESuccess) {
				return eError;
			}

			return this->bind();
		}

		void close();

	public:
		// (Default: True)
		//	Call this before connect.
		//	When it is true, it uses Iocp.
		void setIsNonBlocking(const bool i_fIsNonBlocking)
		{
			this->m_fIsNonBlocking = i_fIsNonBlocking;
		}

 		void setSocket(const SOCKET i_nSocket)
		{
			this->m_nSocket = i_nSocket;
		}

	protected:
		HANDLE createIocp(bool i_fBindSocket, uint i_iThreadCount) const;
		
		void *retrieveFunction(_GUID i_oGuid) const;

		const SProtocolData &protocolData() const
		{
			return this->m_oProtocolData;
		}

	public:
		const SOCKET &socket() const				{ return this->m_nSocket; }
		bool isValid() const						{ return (this->m_nSocket != (~0)); }
		bool isNotBlocked() const					{ return this->m_fIsNonBlocking; }

	private:
		void clear();

	public:
		void operator=(CSocket *i_pSocket) {
			this->m_nSocket = i_pSocket->m_nSocket;
			CPeer::operator=(i_pSocket);
		
			i_pSocket->m_nSocket = (~0);
		}
		void operator=(CSocket &i_oSocket) { return this->operator=(&i_oSocket); }

	private:
		SOCKET			m_nSocket;
		SProtocolData	m_oProtocolData;
		bool			m_fIsNonBlocking;
	};
}