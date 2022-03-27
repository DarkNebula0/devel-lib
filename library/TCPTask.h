#pragma once
#include <WinSock2.h>

#include "GarbageCollector.h"
#include "Typedef.h"
#include "Global.h"


namespace Network
{
	namespace TCP
	{
		enum ETask : byte
		{
			TAccept,
			TConnect,
			//TDisconnect,
			TRecv,
			TSend,
		};

		// ToDo: Maybe safe session in Task struct as weak ptr? (Have 2 look because of operator new & smart ptr constructor in SRecvTask & SSendTask)
		struct STask
		{
			WSAOVERLAPPED oOverlapped;
			ETask eTask;
			DWORD iProcessedBytes;
			SOCKET nSocket;
		};

		struct SAcceptTask : STask
		{
			char	pBuffer[64];
		};

		struct SConnectTask : STask
		{
		};

		/*struct SDisconnectTask : STask
		{
		};*/

		struct SRecvTask : STask
		{
			DWORD iFlags;
			DWORD iBytesReceived;
			WSABUF oBuf;

			void *operator new(size_t i_nSize, uint i_iRecvBufferSize);
			void operator delete(void *i_pMemory);
			
		private:
			static CGarbageCollector<> s_oGarbageCollector;
		};

		struct SWSASendBuf
		{
			WSABUF oBuf;
			DWORD iRealLength;
		};

		struct SSendTask : STask
		{
			DWORD iFlags;
			DWORD iBytesSent;
			SWSASendBuf oUseBuf;
			SWSASendBuf oBackup;
		
			void *operator new(size_t i_nSize, uint i_iSendBufferSize);
			void operator delete(void *i_pMemory);

			void swapBuffers()
			{
				const SWSASendBuf oTemp = this->oBackup;
				this->oBackup = this->oUseBuf;
				this->oUseBuf = oTemp;
			}

		private:
			static CGarbageCollector<> s_oGarbageCollector;
		};
	}
}
