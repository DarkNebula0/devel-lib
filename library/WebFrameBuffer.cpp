#include "WebFrameBuffer.h"


Network::Web::CWebFrameBuffer::CWebFrameBuffer()
	: m_nFramePos(0), m_nFrameMask(0), m_nPayloadLength(0)
{}

Network::Web::CWebFrameBuffer::CWebFrameBuffer(const char *i_pBuffer, const size_t i_nSize)
	: CWebFrameBuffer()
{
	this->m_oBuffer.push(i_pBuffer, i_nSize);
}

// Get payload length======================================
uint64 Network::Web::CWebFrameBuffer::getPayloadLength( const char *i_pBuffer, int i_nFieldLength )
{
	uint64 nPayloadLength = 0;

	if (i_nFieldLength <= 125)
	{
		nPayloadLength = i_nFieldLength;
	}
	else if (i_nFieldLength == 126)
	{
		nPayloadLength |= (static_cast< uint64 >(i_pBuffer[2]) << 8);
		nPayloadLength |= (static_cast< uint64 >(i_pBuffer[3]) & 0xFF);
		this->m_nFramePos += 2;
	}
	else if (i_nFieldLength == 127)
	{
		nPayloadLength = (i_pBuffer[2] + (i_pBuffer[3] << 8));
		this->m_nFramePos += 8;
	}

	return nPayloadLength;
}

int Network::Web::CWebFrameBuffer::readNextBuffer( const CNetworkPacket &i_oPacket )
{
	uint64 nCopyLength = 0;
	int nRes = 0;

	if (this->m_nPayloadLength == i_oPacket.size())
	{
		nCopyLength = this->m_nPayloadLength;
		this->m_nPayloadLength = 0;
	}
	else if (this->m_nPayloadLength > i_oPacket.size())
	{
		nCopyLength = i_oPacket.size();
		this->m_nPayloadLength -= nCopyLength;
	}
	else
	{
		nCopyLength = this->m_nPayloadLength;
		nRes =	static_cast<int>(i_oPacket.size() - this->m_nPayloadLength);
		this->m_nPayloadLength = 0;
	}

	this->m_oBuffer.push(i_oPacket.buffer(), static_cast<size_t>(nCopyLength));
	
	if (this->m_nPayloadLength)
	{
		return 0;
	}
	if (nRes)
	{
		return nRes;
	}

	return 1;
}

// Use mask================================================
void Network::Web::CWebFrameBuffer::useMask()
{
	byte *pBuffer = reinterpret_cast< byte* >(const_cast< char * >(this->m_oBuffer.buffer()));

	for (size_t i = 0; i < this->m_oBuffer.size(); i++)
	{
		pBuffer[i] = pBuffer[i] ^ (reinterpret_cast< byte* >(&this->m_nFrameMask))[i % 4];
	}
}

// Read frames=============================================
int Network::Web::CWebFrameBuffer::readFrames( const CNetworkPacket &i_oPacket )
{
	if (i_oPacket.size() > 3)
	{
		if (this->m_nPayloadLength)
		{
			return this->readNextBuffer(i_oPacket);
		}

		this->m_nFramePos = 0;

		const char *pBuffer = i_oPacket.buffer();

		const byte nOpcode = (pBuffer[0] & 0x0F);
		const bool bFin = ((pBuffer[0] >> 7) & 0x01);
		const bool bHasMask = ((pBuffer[1] >> 7) & 0x01);

		this->m_nFramePos = 0x02;
		const uint64 nPayloadLength = this->getPayloadLength(pBuffer, (pBuffer[1] & (~0x80)));

		if (bHasMask)
		{
			this->m_nFrameMask = *reinterpret_cast< const int* >(pBuffer + this->m_nFramePos);
			this->m_nFramePos += 4;
		}

		// Check opcode
		{
			if (nOpcode == 0x08)
			{
				return -1;
			}
		}

		// Multipart
		if (i_oPacket.size() < (nPayloadLength + this->m_nFramePos) || !bFin)
		{
			// Reallocate buffer
			this->m_oBuffer.reallocate(static_cast<size_t>(nPayloadLength));
			this->m_nPayloadLength = nPayloadLength;

			this->m_oBuffer.push(pBuffer + this->m_nFramePos, i_oPacket.size() - this->m_nFramePos);

			if (this->m_oBuffer.size() > this->m_nPayloadLength)
			{
				if (bHasMask)
				{
					this->useMask();
				}
				return 1;
			}

			this->m_nPayloadLength -= this->m_oBuffer.size();
			return 0;
		}

		// Siglepart
		{
			this->m_oBuffer.push(pBuffer + this->m_nFramePos, i_oPacket.size() - this->m_nFramePos);

			if (bHasMask)
			{
				this->useMask();
			}
			return 1;
		}
	}

	return -1;
}

// Create frame============================================
CWriteStream Network::Web::CWebFrameBuffer::createFrame() const
{
	CWriteStream oStream(this->size() + 20);	
	const uint64 nSize = static_cast<uint64>(this->size());

	oStream.push<byte>(0x81);
	
	if (nSize <= 125)
	{
		oStream.push<byte>(static_cast<byte>(nSize));
	}
	else if (nSize <= 65535)
	{
		oStream.push<byte>(static_cast<byte>(126));
		oStream.push<byte>(static_cast<byte>((nSize >> 8) & 0xFF));
		oStream.push<byte>(static_cast<byte>(nSize & 0xFF));
	}
	else
	{
		oStream.push<byte>(static_cast<byte>(126));
		for (int i = 7;  i >= 0; i--)
		{
			oStream.push<byte>(static_cast<byte>(((nSize >> 8 * i) & 0xFF)));
		}
	}

	// Mask key
	oStream.push<byte>(18);
	oStream.push<byte>(66);
	oStream.push<byte>(45);
	oStream.push<byte>(78);

	const size_t nPos = oStream.size();
	int nMask = *reinterpret_cast<const int *>(oStream.buffer() + nPos - 4);

	// Set mask bit
	const_cast<char *>(oStream.buffer())[1] |= 1 << 7;
	
	// Push data
	oStream.push(this->m_oBuffer.buffer(), this->size());

	// Mask buffer
	{
		char *pBuffer = const_cast<char *>(oStream.buffer() + nPos);
		for (size_t i = 0; i < this->size(); i++)
		{
			pBuffer[i] = pBuffer[i] ^ (reinterpret_cast<byte*>(&nMask))[i % 4];
		}
	}

	return oStream;
}

// Clear===================================================
void Network::Web::CWebFrameBuffer::clear()
{
	this->m_nFramePos = 0x00;
	this->m_nFrameMask = 0x00;
	this->m_nPayloadLength = 0x00;
	this->m_oBuffer.clear();
}