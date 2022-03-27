#include "TCPTask.h"

#pragma region RecvTask
CGarbageCollector<> Network::TCP::SRecvTask::s_oGarbageCollector;

void *Network::TCP::SRecvTask::operator new(const size_t i_nSize, const uint i_iRecvBufferSize)
{
	SRecvTask *pTask = static_cast<SRecvTask *>(SRecvTask::s_oGarbageCollector.get(false));
	if (!pTask)
	{
		const DWORD iLength = (i_iRecvBufferSize > TCP_SERVER_BUFFER_SIZE_DEFAULT ? i_iRecvBufferSize : TCP_SERVER_BUFFER_SIZE_DEFAULT);

		pTask = static_cast<SRecvTask *>(SRecvTask::s_oGarbageCollector.allocate<sizeof(SRecvTask)>());
		pTask->oBuf.buf = new char[iLength];
		pTask->oBuf.len = iLength;
	}

	pTask->eTask			= ETask::TRecv;
	pTask->iBytesReceived	= 0;
	pTask->iProcessedBytes	= 0;
	pTask->iFlags			= 0;
	pTask->oOverlapped		= WSAOVERLAPPED({ 0 });
	pTask->nSocket			= 0;

	return pTask;
}

void Network::TCP::SRecvTask::operator delete(void *i_pMemory)
{
	if (i_pMemory)
	{
		SRecvTask::s_oGarbageCollector.free(i_pMemory);
	}
}
#pragma endregion


#pragma region SendTask
CGarbageCollector<> Network::TCP::SSendTask::s_oGarbageCollector;

void *Network::TCP::SSendTask::operator new(const size_t i_nSize, const uint i_iSendBufferSize)
{
	constexpr DWORD iLength = TCP_SERVER_BUFFER_SIZE_DEFAULT;
	
	SSendTask *pTask = static_cast<SSendTask *>(SSendTask::s_oGarbageCollector.get(false));
	if (!pTask)
	{
		pTask = static_cast<SSendTask *>(SSendTask::s_oGarbageCollector.allocate<sizeof(SSendTask)>());
		pTask->oUseBuf.oBuf.buf = new char[iLength];
		pTask->oUseBuf.oBuf.len = iLength;
		pTask->oUseBuf.iRealLength = iLength;

		pTask->oBackup.oBuf.buf = nullptr;
		pTask->oBackup.oBuf.len = 0;
		pTask->oBackup.iRealLength = 0;
	}

	// Check if default buffer is not big enough
	if (i_iSendBufferSize > iLength)
	{
		// Check if another buffer is already allocated and big enough
		if (!pTask->oBackup.oBuf.buf || pTask->oBackup.iRealLength < i_iSendBufferSize)
		{
			// If exists and just too small, delete
			delete[] pTask->oBackup.oBuf.buf;

			// Set default buffer to backup
			pTask->oBackup = pTask->oUseBuf;

			// Create new buffer
			pTask->oUseBuf.oBuf.buf = new char[i_iSendBufferSize];
			pTask->oUseBuf.oBuf.len = i_iSendBufferSize;
			pTask->oUseBuf.iRealLength = i_iSendBufferSize;
		} else
		{
			pTask->swapBuffers();
		}
	}

	// For checking buffer size, use the iRealLength. The len has to be equal to packet size.
	pTask->oUseBuf.oBuf.len = i_iSendBufferSize;

	pTask->eTask = ETask::TSend;
	pTask->iBytesSent = 0;
	pTask->iProcessedBytes = 0;
	pTask->iFlags = 0;
	pTask->oOverlapped = WSAOVERLAPPED({ 0 });
	pTask->nSocket = 0;

	return pTask;
}

void Network::TCP::SSendTask::operator delete(void *i_pMemory)
{
	if (i_pMemory)
	{
		SSendTask *pTask = static_cast<SSendTask *>(i_pMemory);

		if (pTask->oBackup.oBuf.buf)
		{
			pTask->swapBuffers();
		}

		SSendTask::s_oGarbageCollector.free(i_pMemory);
	}
}
#pragma endregion