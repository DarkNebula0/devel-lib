#include "TCPHandler.h"
#include "TCPTask.h"


void Network::TCP::CHandler::handle(const std::atomic<bool> &i_fIsRunning) const
{
	DWORD iBytesTransferred;
	size_t *pCompletionKey = nullptr;
	Network::TCP::STask *pTask = nullptr;

	while (i_fIsRunning)
	{
		if (GetQueuedCompletionStatus(this->m_pIocp, &iBytesTransferred, reinterpret_cast<PULONG_PTR>(&pCompletionKey), reinterpret_cast<LPOVERLAPPED *>(&pTask), WSA_INFINITE))
		{
			if (pTask)
			{
				pTask->iProcessedBytes = iBytesTransferred;

				if (pTask->eTask == Network::TCP::ETask::TAccept)
				{
					Network::TCP::SAcceptTask *pAcceptTask = static_cast<Network::TCP::SAcceptTask *>(pTask);
					this->onAccept(pAcceptTask);
				}
				else if (pTask->eTask == Network::TCP::ETask::TConnect)
				{
					Network::TCP::SConnectTask *pConnectTask = static_cast<Network::TCP::SConnectTask *>(pTask);
					this->onConnect(pConnectTask);
					delete pConnectTask;
				}
				/*else if (pTask->eTask == Network::TCP::ETask::TDisconnect)
				{
					Network::TCP::SDisconnectTask *pDisconnectTask = static_cast<Network::TCP::SDisconnectTask *>(pTask);
					this->onDisconnect(pDisconnectTask->nSocket);
					delete pDisconnectTask;
				}*/
				else if (pTask->eTask == Network::TCP::ETask::TRecv)
				{
					Network::TCP::SRecvTask *pRecvTask = static_cast<Network::TCP::SRecvTask *>(pTask);
					if (iBytesTransferred == 0)
					{
						this->onDisconnect(pRecvTask->nSocket);
						delete pRecvTask;
					}
					else
					{
						this->onRecv(pTask->nSocket, pRecvTask);
						delete pRecvTask;
						this->onPostRecv(pTask->nSocket);
					}
				}
				else if (pTask->eTask == Network::TCP::ETask::TSend)
				{
					Network::TCP::SSendTask *pSendTask = static_cast<Network::TCP::SSendTask *>(pTask);
					this->onSend(pTask->nSocket, pSendTask);
					delete pSendTask;
				}
			}
		}
		else
		{
			if (pTask)
			{
				// Don't delete AcceptTask! It's a smart pointer in CTCPServer.
				if (pTask->eTask == Network::TCP::ETask::TAccept)
				{ }
				else if (pTask->eTask == Network::TCP::ETask::TConnect)
				{
					delete static_cast<SConnectTask *>(pTask);
					this->onConnectFailed();
				}
				else if (pTask->eTask == Network::TCP::ETask::TRecv)
				{
					const SOCKET nSocket = pTask->nSocket;
					delete static_cast<SRecvTask *>(pTask);
					this->onDisconnect(nSocket);
				}
				else if (pTask->eTask == Network::TCP::ETask::TSend)
				{
					delete static_cast<SSendTask *>(pTask);
				}
			}
		}
	}
}