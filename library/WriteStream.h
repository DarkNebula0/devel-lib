#pragma once
#include "CharArray.h"

#include <type_traits>
#include <string>

class CWriteStream
{
public:
	CWriteStream()
		: m_pBuffer(nullptr), m_nSize(0), m_nAllocatedSize(0)
	{
	}

	CWriteStream(const size_t i_nSize)
		: CWriteStream()
	{
		this->reallocate(i_nSize);
	}

	CWriteStream(const CWriteStream &i_oOther) : CWriteStream()
		{this->operator=(i_oOther);}
	CWriteStream(CWriteStream &&i_oWriteStream) noexcept
	{
		this->operator=(std::move(i_oWriteStream));
	}

	virtual ~CWriteStream()
	{
		this->deleteBuffer();
	}

private:
	void deleteBuffer();

public:
	void clear()
	{
		this->m_nSize = 0;
	}

	void reallocate(size_t i_nSize);
	
public:
	void push(const void *i_pBuffer, size_t i_nSize);
	void push(const CWriteStream &i_oStream)
	{
		if (i_oStream.size() > 0) {
			return this->push(i_oStream.buffer(), i_oStream.size());
		}
	}
	void push(const char *i_szString)
	{
		return this->push(i_szString, strlen(i_szString));
	}
	void push(const std::string &i_oString, const bool i_fZeroTerminated = true)
	{
		if (!i_oString.empty())
		{
			return this->push(i_oString.c_str(), i_oString.size() + (i_fZeroTerminated));
		}
	}
	void push(const std::wstring &i_oString, const bool i_fZeroTerminated = true)
	{
		if (!i_oString.empty())
		{
			return this->push(i_oString.c_str(), i_oString.size() * 2 + (i_fZeroTerminated * 2));
		}
	}

	template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>> * = nullptr>
	void push(const T &i_oValue)
	{
		this->reallocate(this->m_nSize + sizeof(T));

		*reinterpret_cast<T*>((this->m_pBuffer + this->m_nSize)) = i_oValue;
		this->m_nSize += sizeof(T);
	}

	template <typename T, typename std::enable_if_t<std::is_enum_v<T>> * = nullptr>
	void push(const T &i_oValue)
	{
		this->reallocate(this->m_nSize + sizeof(T));

		*reinterpret_cast<T*>((this->m_pBuffer + this->m_nSize)) = i_oValue;
		this->m_nSize += sizeof(T);
	}

	template<size_t TSize>
	void push(const CCharArray<TSize> &i_oValue, const bool i_bMaxLength = true)
	{
		this->push(i_oValue.begin(), i_bMaxLength ? i_oValue.maxLength() : i_oValue.length());
	}

public:
	void replace(const size_t i_nPosition, const void *i_pBuffer, const size_t i_nSize) const
	{
		if (this->m_nSize >= i_nPosition + i_nSize)
		{
			memcpy(this->m_pBuffer + i_nPosition, i_pBuffer, i_nSize);
		} else
		{
			throw CBufferOutOfRangeException();
		}
	}
	void replace(const size_t i_nPosition, const CWriteStream &i_oStream) const
	{
		if (i_oStream.size() > 0)
		{
			this->replace(i_nPosition, i_oStream.buffer(), i_oStream.size());
		}
	}
	void replace(const size_t i_nPosition, const char *i_szString) const
	{
		return this->replace(i_nPosition,i_szString,strlen(i_szString));
	}
	void replace(const size_t i_nPosition, const std::string &i_oString) const
	{
		return this->replace(i_nPosition, i_oString.c_str(), i_oString.size());
	}

	template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>> * = nullptr>
	void replace(const size_t i_nPosition, const T &i_oValue)
	{
		if (this->m_nSize >= i_nPosition + sizeof(T))
		{
			*reinterpret_cast<T*>((this->m_pBuffer + i_nPosition)) = i_oValue;
		} else
		{
			throw CBufferOutOfRangeException();
		}
	}

	template <typename T, typename std::enable_if_t<std::is_enum_v<T>> * = nullptr>
	void replace(const size_t i_nPosition, const T &i_oValue)
	{
		if (this->m_nSize >= i_nPosition + sizeof(T))
		{
			*reinterpret_cast<T*>((this->m_pBuffer + i_nPosition)) = i_oValue;
		}
		else
		{
			throw CBufferOutOfRangeException();
		}
	}

public:
	size_t allocatedSize() const
	{
		return this->m_nAllocatedSize;
	}

	size_t size() const
	{
		return this->m_nSize;
	}

	const char *buffer() const
	{
		return this->m_pBuffer;
	}

public:
	CWriteStream &operator=(const CWriteStream &i_oOther)
	{
		this->clear();
		this->push(i_oOther.m_pBuffer, i_oOther.m_nSize);
		return *this;
	}
	CWriteStream &operator=(CWriteStream &&i_oOther) noexcept
	{
		this->deleteBuffer();
		
		this->m_pBuffer			= i_oOther.m_pBuffer;
		this->m_nAllocatedSize	= i_oOther.m_nAllocatedSize;
		this->m_nSize			= i_oOther.m_nSize;

		i_oOther.m_pBuffer = nullptr;
		i_oOther.m_nAllocatedSize = 0;
		i_oOther.m_nSize = 0;

		return *this;
	}

private:
	char *m_pBuffer;
	size_t m_nSize;
	size_t m_nAllocatedSize;
};