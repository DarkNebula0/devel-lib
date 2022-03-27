#pragma once
#include <string>
#include "Typedef.h"

namespace Network
{
	class CPacket
	{
	public:
		CPacket(const char *i_pBuffer, const size_t i_nSize, const bool i_bDeleteBuffer = false, const bool i_fCopyBuffer = false)
			: m_pBuffer(i_pBuffer), m_nSize(i_nSize)
			, m_bDeleteBuffer(i_bDeleteBuffer)
		{
			if (i_fCopyBuffer)
			{
				this->m_pBuffer = new char[i_nSize];
				memcpy(const_cast<char *>(this->m_pBuffer), i_pBuffer, i_nSize);
			}
		}	
		CPacket(const CPacket &i_oOther)
			: CPacket(i_oOther, true, true)
		{}
		CPacket(CPacket &&i_oPacket) noexcept
		{
			this->m_pBuffer       = i_oPacket.m_pBuffer;
			this->m_nSize         = i_oPacket.m_nSize;
			this->m_bDeleteBuffer = i_oPacket.m_bDeleteBuffer;

			i_oPacket.m_pBuffer = nullptr;
			i_oPacket.m_nSize   = 0;
		}

		CPacket(const CPacket &i_oPacket, const bool i_bDeleteBuffer, const bool i_fCopyBuffer)
			: CPacket(i_oPacket.buffer(), i_oPacket.size(), i_bDeleteBuffer, i_fCopyBuffer)
		{}
		CPacket(const size_t i_nSize)
			: CPacket(new char[i_nSize], i_nSize, true)
		{}
		virtual ~CPacket()
		{
			if (this->m_bDeleteBuffer && this->m_pBuffer)
				delete[] this->m_pBuffer;
		}

	public:
		template <typename T>
		const T *to(size_t i_nIndex = 0) const
		{
			return reinterpret_cast<const T *>(this->m_pBuffer + i_nIndex);
		}

	public:
		const char *buffer() const
		{
			return this->m_pBuffer;
		}

		size_t size() const
		{
			return this->m_nSize;
		}

	public:
		void setDeleteBuffer(const bool i_bState)
		{
			this->m_bDeleteBuffer = i_bState;
		}

	private:
		const char *m_pBuffer;
		size_t m_nSize;
		bool m_bDeleteBuffer;
	};
}

typedef Network::CPacket CNetworkPacket;
