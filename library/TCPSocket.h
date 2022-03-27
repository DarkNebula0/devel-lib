#pragma once
#include "NetworkSocket.h"
#include "NetworkPacket.h"

namespace Network {
	namespace TCP {
		class CSocket : public Network::CSocket
		{
			friend class Network::TCP::CClient;
			friend class Network::TCP::CServer;
		public:
			CSocket() = default;
			CSocket(const std::string &i_sIp, const ushort i_wPort) : CSocket() { this->setAddress(i_sIp, i_wPort); }
			CSocket(Network::TCP::CSocket *i_pSocket) : CSocket() { this->operator=(i_pSocket); }
			CSocket(Network::TCP::CSocket &i_oSocket) : CSocket() { this->operator=(i_oSocket); }
			virtual ~CSocket() = default;

		public:
			bool send(const char *i_pPacket, uint i_iSize, ESendFlag i_eFlag = ESendFlag::SFStandard) const;
			bool send(const Network::CPacket &i_oPacket, const ESendFlag i_eFlag = ESendFlag::SFStandard) const	{ return this->send(&i_oPacket, i_eFlag); }
			bool send(const Network::CPacket *i_pPacket, const ESendFlag i_eFlag = ESendFlag::SFStandard) const	{ return this->send(i_pPacket->buffer(), static_cast<uint>(i_pPacket->size()), i_eFlag); }

			uint recv(char *i_pPacket, uint i_iMaxSize, ERecvFlag i_eFlag = ERecvFlag::RFStandard) const;
			uint recv(Network::CPacket &i_oPacket, const ERecvFlag i_eFlag = ERecvFlag::RFStandard) const	{ return this->recv(&i_oPacket, i_eFlag); }
			uint recv(Network::CPacket *i_pPacket, const ERecvFlag i_eFlag = ERecvFlag::RFStandard) const	{ return this->recv(const_cast<char *>(i_pPacket->buffer()), static_cast<uint>(i_pPacket->size()), i_eFlag); }

		public:
			virtual void onPacketReceived(const Network::CPacket &i_oPacket) { }
			virtual void onDisconnected() { }

		private:
			// To call startSend, set NonBlocking to true(It's default), and call send()
			bool startSend(const char *i_pPacket, uint i_iSize, ESendFlag i_eFlag = ESendFlag::SFStandard) const;

			// Recv is automatically
			void startRecv(uint i_iReceiveBufferSize = TCP_SERVER_BUFFER_SIZE_DEFAULT, ERecvFlag i_eFlag = ERecvFlag::RFStandard) const;

		public:
			void operator=(Network::TCP::CSocket &i_oSocket) { return this->operator=(&i_oSocket); }
			void operator=(Network::TCP::CSocket *i_pSocket) { return Network::CSocket::operator=(i_pSocket); }
		};
	}
}