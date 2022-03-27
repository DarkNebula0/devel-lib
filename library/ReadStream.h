#pragma once
#include "CharArray.h"
#include "Typedef.h"
#include "Global.h"

#include <type_traits>
#include <string>

class CReadStream
{
public:
	CReadStream()
		: m_pBuffer(nullptr), m_nSize(0), m_nPosition(0), m_fDeleteBuffer(false)
	{
		this->clear();
	}

	CReadStream(const char *i_pBuffer, const size_t i_nSize, const bool i_fCopyBuffer)
		: CReadStream()
	{
		this->setBuffer(i_pBuffer, i_nSize, i_fCopyBuffer);
	}

	CReadStream(const Network::CPacket *i_pPacket, const bool i_fCopyBuffer)
	{
		this->setBuffer(i_pPacket, i_fCopyBuffer);
	}

	CReadStream(const Network::CPacket &i_oPacket, const bool i_fCopyBuffer)
		: CReadStream(&i_oPacket, i_fCopyBuffer)
	{
	}

	CReadStream(const CReadStream &i_oOther)
	{
		this->operator=(i_oOther);
	}

	CReadStream(CReadStream &&i_oOther) noexcept
	{
		this->operator=(std::move(i_oOther));
	}

	virtual ~CReadStream() = default;

private:
	void deleteBuffer();

private:
	bool checkBuffer() const
	{
		if (!this->m_pBuffer)
		{
			throw CNoBufferException();
		}

		return true;
	}

	bool checkSize(const size_t i_nPosition, const size_t i_nReadBytes) const
	{
		if (this->leftBytes(i_nPosition) < i_nReadBytes)
		{
			throw CBufferOutOfRangeException();
		}

		return true;
	}

	bool checkBufferAndSize(const size_t i_nPosition, const size_t i_nReadBytes) const
	{
		return this->checkBuffer() && this->checkSize(i_nPosition, i_nReadBytes);
	}


	char *bufferPosition(const size_t i_nPosition) const
	{
		return const_cast<char *>(this->m_pBuffer + i_nPosition);
	}

public:
	void setBuffer(const char *i_pBuffer, size_t i_nSize, bool i_fCopyBuffer = false);

	void setBuffer(const Network::CPacket *i_pPacket, bool i_fCopyBuffer = false);

	void setBuffer(const Network::CPacket &i_oPacket, const bool i_fCopyBuffer = false)
	{
		this->setBuffer(&i_oPacket, i_fCopyBuffer);
	}

	void setPosition(const size_t i_nPosition)
	{
		if (this->m_pBuffer)
		{
			this->m_nPosition = i_nPosition > this->m_nSize ? this->m_nSize : i_nPosition;
		}
	}

	void seek(const size_t i_nBytes)
	{
		if (i_nBytes > 0)
		{
			this->setPosition(this->position() + i_nBytes);
		}
	}

	void clear();

public:
	const char *buffer() const
	{
		return this->m_pBuffer;
	}

	size_t size() const
	{
		return this->m_nSize;
	}

	size_t position() const
	{
		return this->m_nPosition;
	}

	size_t leftBytes(const size_t i_nPosition) const
	{
		if (this->m_pBuffer)
		{
			return this->m_nSize - i_nPosition;
		}

		return 0;
	}

	size_t leftBytes() const
	{
		return this->leftBytes(this->m_nPosition);
	}

	bool isEndOfBuffer() const
	{
		return (this->m_nPosition == this->size());
	}

	bool isEoB() const
	{
		return this->isEndOfBuffer();
	}

private:
	template<typename TReturnType, typename TType>
	TReturnType readString(const size_t i_nPosition, size_t i_nLength, bool i_fIsZeroTerminated, const bool i_fSetPosition)
	{
		TReturnType sReturn;

		if (this->checkBuffer())
		{
			if (i_fIsZeroTerminated)
			{
				const size_t nLeftBytes = this->leftBytes(i_nPosition);
				if (nLeftBytes > 0)
				{
					// Set to false for check if found
					i_fIsZeroTerminated = false;
					i_nLength = 0;

					sReturn.reserve((nLeftBytes > 20 ? 20 : nLeftBytes));

					for (; i_nLength < nLeftBytes; i_nLength += sizeof(TType))
					{
						TType *pBufferPosition = reinterpret_cast<TType *>(this->bufferPosition(this->m_nPosition + i_nLength));
						if (*pBufferPosition != TType(0x00))
						{
							sReturn += *pBufferPosition;
						} else
						{
							i_fIsZeroTerminated = true;
							break;
						}
					}

					if (!i_fIsZeroTerminated)
					{
						throw CZeroTerminationNotFound();
					}
				}
			}
			else if (i_nLength > 0 && this->checkSize(i_nPosition, i_nLength))
			{
				sReturn = TReturnType(reinterpret_cast<TType *>(this->bufferPosition(i_nPosition)), i_nLength);
				i_nLength *= sizeof(TType);	// For wchar_t (x characters = x * 2 bytes)
			}

			if (i_fSetPosition)
			{
				this->setPosition(i_nPosition + i_nLength + (i_fIsZeroTerminated * sizeof(TType)));
				//this->seek(i_nLength + (i_fIsZeroTerminated * sizeof(TType)));
			}
		}

		return sReturn;
	}

public:
	std::string getString(const size_t i_nPosition, const size_t i_nLength, const bool i_fIsZeroTerminated, const bool i_fSetPosition)
	{
		return this->readString<std::string, char>(i_nPosition, i_nLength, i_fIsZeroTerminated, i_fSetPosition);
	}

	std::string getString(const size_t i_nPosition, const size_t i_nLength, const bool i_fSetPosition = true)
	{
		return this->getString(i_nPosition, i_nLength, false, i_fSetPosition);
	}

	std::string getString(const size_t i_nLength, const bool i_fSetPosition = true)
	{
		return this->getString(this->m_nPosition, i_nLength, false, i_fSetPosition);
	}

	std::string getString(const bool i_fSetPosition = true)
	{
		return this->getString(this->m_nPosition, 0, true, i_fSetPosition);
	}

public:
	std::wstring getWString(const size_t i_nPosition, const size_t i_nLength, const bool i_fIsZeroTerminated, const bool i_fSetPosition)
	{
		return this->readString<std::wstring, wchar_t>(i_nPosition, i_nLength, i_fIsZeroTerminated, i_fSetPosition);
	}

	std::wstring getWString(const size_t i_nPosition, const size_t i_nLength, const bool i_fSetPosition = true)
	{
		return this->getWString(i_nPosition, i_nLength, false, i_fSetPosition);
	}

	std::wstring getWString(const size_t i_nLength, const bool i_fSetPosition = true)
	{
		return this->getWString(this->m_nPosition, i_nLength, false, i_fSetPosition);
	}

	std::wstring getWString(const bool i_fSetPosition = true)
	{
		return this->getWString(this->m_nPosition, 0, true, i_fSetPosition);
	}

public:
	void getRaw(void *i_pDestination, const size_t i_nReadBytes, const size_t i_nPosition, const bool i_fSetPosition);

	void getRaw(void *i_pDestination, const size_t i_nReadBytes, const bool i_fSetPosition)
	{
		return this->getRaw(i_pDestination, i_nReadBytes, this->m_nPosition, i_fSetPosition);
	}

	void getRaw(void *i_pDestination, const size_t i_nReadBytes)
	{
		return this->getRaw(i_pDestination, i_nReadBytes, true);
	}

public:
	template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>> * = nullptr>
	T get(const size_t i_nPosition, const bool i_fSetPosition)
	{
		if (this->checkBufferAndSize(i_nPosition, sizeof(T)))
		{
			T tReturn = *reinterpret_cast<T *>(this->bufferPosition(i_nPosition));

			if (i_fSetPosition)
			{
				this->setPosition(i_nPosition + sizeof(T));
			}

			return tReturn;
		}

		throw CShouldNotExecuteException();
	}

	template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
	T get(const size_t i_nPosition)
	{
		return this->get<T>(this->m_nPosition, true);
	}

	template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
	T get()
	{
		return this->get<T>(this->m_nPosition);
	}


	template <typename T, typename std::enable_if_t<std::is_enum_v<T>> * = nullptr>
	T get(const size_t i_nPosition)
	{
		return static_cast<T>(this->get<std::underlying_type_t<T>>(this->m_nPosition, true));
	}

	template <typename T, typename std::enable_if_t<std::is_enum_v<T>> * = nullptr>
	T get()
	{
		return static_cast<T>(this->get<std::underlying_type_t<T>>(this->m_nPosition));
	}

public:
	template <size_t TSize>
	CCharArray<TSize> getArray(const size_t i_nPosition, const bool i_fSetPosition)
	{
		CCharArray<TSize> oBuffer;
		
		this->getRaw(oBuffer.begin(), TSize, i_nPosition, i_fSetPosition);
		
		return oBuffer;
	}

	template <size_t TSize>
	CCharArray<TSize> getArray()
	{
		return this->getArray<TSize>(this->m_nPosition, true);
	}

public:
	const char &operator[](const size_t i_nIndex) const
	{
		if (this->checkBufferAndSize(i_nIndex, 1))
		{
			return this->m_pBuffer[i_nIndex];
		}

		throw CShouldNotExecuteException();
	}

	CReadStream &operator=(const CReadStream &i_oOther)
	{
		this->setBuffer(i_oOther.m_pBuffer, i_oOther.m_nSize, true);

		this->m_nPosition     = i_oOther.m_nPosition;
		this->m_fDeleteBuffer = i_oOther.m_fDeleteBuffer;

		return *this;
	}

	CReadStream &operator=(CReadStream &&i_oOther) noexcept
	{
		this->m_pBuffer       = i_oOther.m_pBuffer;
		this->m_nSize         = i_oOther.m_nSize;
		this->m_nPosition     = i_oOther.m_nPosition;
		this->m_fDeleteBuffer = i_oOther.m_fDeleteBuffer;

		i_oOther.m_fDeleteBuffer = false;
		i_oOther.clear();

		return *this;
	}

private:
	const char *m_pBuffer;
	size_t m_nSize;
	size_t m_nPosition;
	bool m_fDeleteBuffer;
};
