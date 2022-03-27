#include "ReadStream.h"
#include "NetworkPacket.h"

// Delete buffer===========================================
void CReadStream::deleteBuffer()
{
	if (this->m_fDeleteBuffer)
		delete[] this->m_pBuffer;

	this->m_pBuffer = nullptr;
}

// Clear===================================================
void CReadStream::clear()
{
	this->m_nPosition = 0x00;
	this->m_nSize     = 0x00;

	this->deleteBuffer();
}

// Set buffer==============================================
void CReadStream::setBuffer(const char *i_pBuffer, const size_t i_nSize, const bool i_fCopyBuffer)
{
	this->clear();

	this->m_nSize = i_nSize;

	if (i_fCopyBuffer)
	{
		this->m_pBuffer       = new char[i_nSize];
		this->m_fDeleteBuffer = true;

		memcpy(const_cast<char*>(this->m_pBuffer), i_pBuffer, i_nSize);
	}
	else
	{
		this->m_pBuffer       = i_pBuffer;
		this->m_fDeleteBuffer = false;
	}
}

void CReadStream::setBuffer(const Network::CPacket *i_pPacket, const bool i_fCopyBuffer)
{
	if (i_pPacket)
	{
		return this->setBuffer(i_pPacket->buffer(), i_pPacket->size(), i_fCopyBuffer);
	}
}

// Get raw=================================================
void CReadStream::getRaw(void *i_pDestination, const size_t i_nReadBytes, const size_t i_nPosition, const bool i_fSetPosition)
{
	if (this->checkBufferAndSize(i_nPosition, i_nReadBytes))
	{
		memcpy(i_pDestination, this->bufferPosition(i_nPosition), i_nReadBytes);

		if (i_fSetPosition)
		{
			this->setPosition(i_nPosition + i_nReadBytes);
			//this->seek(i_nReadBytes);
		}
	}
}