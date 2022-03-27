#include "WriteStream.h"


void CWriteStream::deleteBuffer()
{
	delete[] this->m_pBuffer;

	this->m_pBuffer = nullptr;
	this->m_nSize = 0;
	this->m_nAllocatedSize = 0;
}

void CWriteStream::reallocate(const size_t i_nSize)
{
	if (i_nSize <= this->m_nAllocatedSize)
		return;

	size_t nReserveSize = i_nSize / 3;
	if (nReserveSize > 500)
		nReserveSize = 500;

	char *pOld             = this->m_pBuffer;
	this->m_nAllocatedSize = i_nSize + nReserveSize;
	this->m_pBuffer        = new char[this->m_nAllocatedSize];

	if (pOld != nullptr)
	{
		memcpy(this->m_pBuffer, pOld, this->m_nSize);
		delete[] pOld;
	}
}

void CWriteStream::push(const void *i_pBuffer, const size_t i_nSize)
{
	this->reallocate(this->m_nSize + i_nSize);
	memcpy(this->m_pBuffer + this->m_nSize, i_pBuffer, i_nSize);
	this->m_nSize += i_nSize;
}
