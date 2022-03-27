#pragma once

#include "Typedef.h"

#include "NetworkPacket.h"
#include "WriteStream.h"

namespace Network
{
	namespace Web
	{
		class CWebFrameBuffer
		{
			friend class CWebSocket;
		public:
			CWebFrameBuffer();
			CWebFrameBuffer(const char* i_pBuffer,const size_t i_nSize);
			virtual ~CWebFrameBuffer() = default;
		private:
			CWriteStream m_oBuffer;
		private:
			size_t	m_nFramePos;
			int		m_nFrameMask;
			uint64	m_nPayloadLength;
		private:
			uint64 getPayloadLength(const char *i_pBuffer, int i_nFieldLength );
			int readNextBuffer(const CNetworkPacket &i_oPacket);
			void useMask();
		private:
			// -1 Failed | 0 Incomplete | 1 Complete
			int readFrames(const CNetworkPacket &i_oPacket);
		private:
			CWriteStream createFrame() const;
		public:
			void clear();
		public:
			_inline void pushBuffer(const char *i_pBuffer, const size_t i_nSize )
			{
				this->m_oBuffer.push(i_pBuffer, i_nSize);
			}
		public:
			_inline const char* buffer() const { return this->m_oBuffer.buffer(); }
			_inline size_t size() const { return this->m_oBuffer.size(); }
		private:
			_inline CNetworkPacket getPacket()
			{
				CNetworkPacket oPacket(this->m_oBuffer.buffer(), static_cast<uint>(this->m_oBuffer.size()), true, true);
				this->clear();
				return oPacket;
			}
		};
	}
}