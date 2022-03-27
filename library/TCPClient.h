#pragma once
#include <functional>
#include <atomic>

#include "NetworkThread.h"
#include "TCPHandler.h"
#include "TCPSocket.h"

namespace Network {
	namespace TCP {
		/* When this socket is set to blocked, you have to call send & recv yourself! */
		enum EState : byte
		{
			SDisconnected,
			SConnecting,
			SConnected,
		};
		class CClient : public Network::TCP::CSocket
		{
		public:
			CClient();
			CClient(const std::string &i_sIp, const ushort i_wPort) : CClient() { this->setAddress(i_sIp, i_wPort); }
			virtual ~CClient() { this->stop(); }

		public:
			Network::EError connect(ushort i_wSecondsTimeout = 10, bool i_fExecute = true);
			Network::EError connect(const std::string &i_sIp, const ushort i_wPort, const ushort i_wSecondsTimeout = 10, const bool i_fExecute = true, const uint i_iReceiveBufferSize = TCP_SERVER_BUFFER_SIZE_DEFAULT)
			{
				const Network::EError eError = this->setAddress(i_sIp, i_wPort);
				if (eError != Network::EError::ESuccess)
				{
					return eError;
				}

				this->setReceiveBufferSize(i_iReceiveBufferSize);

				return this->connect(i_wSecondsTimeout, i_fExecute);
			}
			Network::EError connect(const CPeer &i_oEndpoint, const ushort i_wSecondsTimeout = 10, const bool i_fExecute = true, const uint i_iReceiveBufferSize = TCP_SERVER_BUFFER_SIZE_DEFAULT)
			{
				return this->connect(i_oEndpoint.ip(), i_oEndpoint.port(), i_wSecondsTimeout, i_fExecute, i_iReceiveBufferSize);
			}

			void execute();

			void stop();

		private:
			Network::EError startConnect(bool i_fExecute);

		protected:
			virtual void onConnected();
			virtual void onConnectFailed();
			void onPacketReceived(const Network::CPacket &i_oPacket) override;
			void onDisconnected() override;

		public:
			void setConnectCallback(const std::function<void()> &i_oFunction)
			{
				this->m_fnConnected = i_oFunction;
			} 
			void setConnectFailedCallback(const std::function<void()> &i_oFunction)
			{
				this->m_fnConnectFailed = i_oFunction;
			} 
			void setRecvCallback(const std::function<void(const Network::CPacket &i_oPacket)> &i_oFunction)
			{
				this->m_fnRecv = i_oFunction;
			}
			void setDisconnectCallback(const std::function<void()> &i_oFunction)
			{
				this->m_fnDisconnected = i_oFunction;
			}

			void setReceiveBufferSize(const uint i_iReceiveBufferSize = TCP_SERVER_BUFFER_SIZE_DEFAULT)
			{
				if (!this->m_eState == EState::SDisconnected)
				{
					this->m_iReceiveBufferSize = (i_iReceiveBufferSize == 0 ? 1 : i_iReceiveBufferSize);
				}
			}

		public:
			uint receiveBufferSize() const
			{
				return this->m_iReceiveBufferSize;
			}

			EState state() const
			{
				return this->m_eState;
			}

			bool isConnected() const
			{
				return this->m_eState == EState::SConnected;
			}

			bool isConnecting() const
			{
				return this->m_eState == EState::SConnecting;
			}

			bool isDisconnected() const
			{
				return this->m_eState == EState::SDisconnected;
			}

		private:
			void onConnect(Network::TCP::SConnectTask *i_pTask);
			void onRecv(Network::SOCKET i_nSocket, Network::TCP::SRecvTask *i_pTask);
			void onSend(Network::SOCKET i_nSocket, Network::TCP::SSendTask *i_pTask) const;
			void onDisconnect(Network::SOCKET i_nSocket);

		private:
			Function												m_fnConnectEx;

			std::atomic<EState>										m_eState;
			std::atomic<bool>										m_fIsExecuted;

			HANDLE													m_oIocp;

			uint													m_iReceiveBufferSize;
			ushort													m_wSecondsConnectTimeout;

			std::function<void()>									m_fnConnected;
			std::function<void()>									m_fnConnectFailed;
			std::function<void(const Network::CPacket &i_oPacket)>	m_fnRecv;
			std::function<void()>									m_fnDisconnected;

			std::thread												m_oHandleThread;
			CHandler												m_oHandler;

			CPeer													m_oBindPeer;
		};
	}
}