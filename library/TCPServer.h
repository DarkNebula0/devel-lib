#pragma once
#include "NetworkSocket.h"
#include "MutexVector.h"
#include "ThreadPool.h"
#include "ObjectData.h"
#include "TCPHandler.h"

#include <atomic>

/*	Example:
*	onPacketReceived: Both virtual functions getting called (In TCPServer and in TCPSocket). So you can decide, where you want to handle your packet!
*	onDisconnected: Same like onPacketReceived

class CMyCustomSocket : public CTCPSocket { };
class CMyTCPServer : public CTCPServer
{
	// DON'T FORGET THIS! In background the TCP-Server class will create the passed template-class. So you need an empty constructor in your custom class!
	CMyTCPServer() { this->setSocketClass<CMyCustomSocket>(); }

	// Same with Disconnected, Recv and Send (Cast the socket with "CastShared" to your custom class)
	void onNewConnection(const TCPSocket &i_pSocket) override {
		std::shared_ptr<CMyCustomSocket> pSocket = CastShared(CMyCustomSocket, i_pSocket);
	}
};
*/

namespace Network
{
	namespace TCP
	{
		class CServer : public Network::CSocket
		{
		public:
			CServer();
			CServer(const std::string &i_sIp, const ushort i_wPort)
				: CServer()
			{
				this->setAddress(i_sIp, i_wPort);
			}
			virtual ~CServer();

		public:
			virtual void onNewConnection(TCPSocket i_pSocket) {}
			virtual void onPacketReceived(TCPSocket i_pSocket, const Network::CPacket &i_oPacket) {}
			virtual void onDisconnected(TCPSocket i_pSocket) {}

		public:
			Network::EError start(const bool i_fExecute);
			Network::EError start(const bool i_fExecute, const uint i_iReceiveBufferSize, const uint i_iThreadCount, const uint i_iMaxConnections)
			{
				this->setReceiveBufferSize(i_iReceiveBufferSize);
				this->setThreadCount(i_iThreadCount);
				this->setMaxConnections(i_iMaxConnections);

				return this->start(i_fExecute);
			}
			Network::EError start(const std::string &i_sIp, const ushort i_wPort, const bool i_fExecute = true, const uint i_iReceiveBufferSize = TCP_SERVER_BUFFER_SIZE_DEFAULT, const uint i_iThreadCount = TCP_SERVER_THREADS_DEFAULT, const uint i_iMaxConnections = TCP_SERVER_MAX_CONNECTIONS_DEFAULT)
			{
				const Network::EError eError = this->setAddress(i_sIp, i_wPort);
				if (eError != Network::EError::ESuccess)
				{
					return eError;
				}

				return this->start(i_fExecute, i_iReceiveBufferSize, i_iThreadCount, i_iMaxConnections);
			}
			Network::EError start(const Network::SEndpoint &i_oEndpoint, const bool i_fExecute = true, const uint i_iReceiveBufferSize = TCP_SERVER_BUFFER_SIZE_DEFAULT, const uint i_iThreadCount = TCP_SERVER_THREADS_DEFAULT, const uint i_iMaxConnections = TCP_SERVER_MAX_CONNECTIONS_DEFAULT)
			{
				return this->start(i_oEndpoint.sIp, i_oEndpoint.wPort, i_fExecute, i_iReceiveBufferSize, i_iThreadCount, i_iMaxConnections);
			}

			void execute();

			void stop();

		public:
			template<typename TSocketClass, typename std::enable_if_t<std::is_base_of_v<Network::TCP::CSocket, TSocketClass>> * = nullptr>
			void setSocketClass()
			{
				if (!this->m_fIsRunning)
				{
					this->m_oSocketClassData = CObjectData::get<TSocketClass>();
				}
			}

			void setReceiveBufferSize(const uint i_iReceiveBufferSize = TCP_SERVER_BUFFER_SIZE_DEFAULT)
			{
				if (!this->m_fIsRunning)
				{
					this->m_iReceiveBufferSize = (i_iReceiveBufferSize == 0 ? 1 : i_iReceiveBufferSize);
				}
			}

			void setThreadCount(const uint i_iThreadCount = TCP_SERVER_THREADS_DEFAULT)
			{
				if (!this->m_fIsRunning)
				{
					this->m_iThreadCount = (i_iThreadCount == 0 ? 1 : i_iThreadCount);
				}
			}

			void setMaxConnections(const uint i_iMaxConnections = TCP_SERVER_MAX_CONNECTIONS_DEFAULT)
			{
				if (!this->m_fIsRunning)
				{
					this->m_iMaxConnections = (i_iMaxConnections == 0 ? 1 : i_iMaxConnections);
				}
			}

		public:
			TCPSocket socket(Network::SOCKET i_nSocket) const;

			bool isRunning() const
			{
				return this->m_fIsRunning;
			}

			uint receiveBufferSize() const
			{
				return this->m_iReceiveBufferSize;
			}

			uint threadCount() const
			{
				return this->m_iThreadCount;
			}

			uint maxConnections() const
			{
				return this->m_iMaxConnections;
			}

		private:
			void startAccept();
			void executeThreadPool();

		private:
			void onAccept(Network::TCP::SAcceptTask *i_pTask);
			void onRecv(Network::SOCKET i_nSocket, Network::TCP::SRecvTask *i_pTask);
			void onSend(Network::SOCKET i_nSocket, Network::TCP::SSendTask *i_pTask) const;
			void onDisconnect(Network::SOCKET i_nSocket);

		private:
			Network::EError retrieveFunctions();

		private:
			Function									m_fnAcceptEx;
			Function									m_fnGetAcceptExSockAddrs;

			std::atomic<bool>							m_fIsRunning;
			std::atomic<bool>							m_fIsAccepting;

			HANDLE										m_pIocp;
			std::unique_ptr<Network::TCP::SAcceptTask>	m_pAcceptTask;
			CObjectData									m_oSocketClassData;

			uint										m_iReceiveBufferSize;
			uint										m_iMaxConnections;
			uint										m_iThreadCount;

			CHandler									m_oHandler;
			CThreadPool									m_oThreadPool;

			CMutexVector<TCPSocket>						m_apSockets;
		};
	}
}