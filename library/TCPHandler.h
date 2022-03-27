#pragma once
#include <functional>
#include <atomic>

#include "Typedef.h"

namespace Network
{
	namespace TCP
	{
		class CHandler
		{
			typedef std::function<void(Network::TCP::SAcceptTask *)> FnOnAccept;
			typedef std::function<void(Network::TCP::SConnectTask *)> FnOnConnect;
			typedef std::function<void(Network::SOCKET, Network::TCP::SRecvTask *)> FnOnRecv;
			typedef std::function<void(Network::SOCKET, Network::TCP::SSendTask *)> FnOnSend;
			typedef std::function<void(Network::SOCKET)> FnOnDisconnect;

		public:
			CHandler()
				: m_pIocp(nullptr)
			{}

			virtual ~CHandler() = default;

		public:
			void setCompletionPort(const HANDLE i_pIocp)
			{
				this->m_pIocp = i_pIocp;
			}

			void setAcceptCallback(const FnOnAccept i_fnOnAccept)
			{
				this->m_fnOnAccept = i_fnOnAccept;
			}

			// When the parameter in the callback function is nullptr, the connection was unsuccessfully
			// Else it was successfully
			void setConnectCallback(const FnOnConnect i_fnOnConnect)
			{
				this->m_fnOnConnect = i_fnOnConnect;
			}

			// When the second parameter of the callback is nullptr, it is the post-recv (mostly used for restart an Iocp-Task like recv in this case)
			void setRecvCallback(const FnOnRecv i_fnOnRecv)
			{
				this->m_fnOnRecv = i_fnOnRecv;
			}

			void setSendCallback(const FnOnSend i_fnOnSend)
			{
				this->m_fnOnSend = i_fnOnSend;
			}

			void setDisconnectCallback(const FnOnDisconnect i_fnOnDisconnect)
			{
				this->m_fnOnDisconnect = i_fnOnDisconnect;
			}

		public:
			void handle(const std::atomic<bool> &i_fIsRunning) const;

		private:
			void onAccept(Network::TCP::SAcceptTask *i_pTask) const
			{
				if (this->m_fnOnAccept)
				{
					this->m_fnOnAccept(i_pTask);
				}
			}

			void onConnect(Network::TCP::SConnectTask *i_pTask) const
			{
				if (this->m_fnOnConnect)
				{
					this->m_fnOnConnect(i_pTask);
				}
			}

			void onConnectFailed() const
			{
				if (this->m_fnOnConnect)
				{
					this->m_fnOnConnect(nullptr);
				}
			}

			void onRecv(const Network::SOCKET i_nSocket, Network::TCP::SRecvTask *i_pTask) const
			{
				if (this->m_fnOnRecv)
				{
					this->m_fnOnRecv(i_nSocket, i_pTask);
				}
			}
			
			void onPostRecv(const Network::SOCKET i_nSocket) const
			{
				if (this->m_fnOnRecv)
				{
					this->m_fnOnRecv(i_nSocket, nullptr);
				}
			}

			void onSend(const Network::SOCKET i_nSocket, Network::TCP::SSendTask *i_pTask) const
			{
				if (this->m_fnOnSend)
				{
					this->m_fnOnSend(i_nSocket, i_pTask);
				}
			}

			void onDisconnect(const Network::SOCKET i_nSocket) const
			{
				if (this->m_fnOnDisconnect)
				{
					this->m_fnOnDisconnect(i_nSocket);
				}
			}

		private:
			HANDLE			m_pIocp;

			FnOnAccept		m_fnOnAccept;
			FnOnConnect		m_fnOnConnect;
			FnOnRecv		m_fnOnRecv;
			FnOnSend		m_fnOnSend;
			FnOnDisconnect	m_fnOnDisconnect;
		};
	}
}
