#pragma once
#include <istream>
#include <ostream>
#include <vector>
#include <string>
#include <functional>

#include "SerializableDefines.h"
#include "WriteStream.h"
#include "ReadStream.h"

namespace Serialize
{
	CreateException(CFalseException,				"Serialize-Function returned false!");
	CreateException(CTooLessDataInPacketException,	"Not enough data in Packet available for deserialize into Struct!");

	struct SCompareData
	{
		SCompareData(const void *i_pData, const size_t i_nSize)
			: pData(i_pData), nSize(i_nSize)
		{}

		const void *pData;
		const size_t nSize;
	};

#pragma pack(push, 1)
	class IStruct { };

	// Needed for output and input stream operator
	class IField {};

	template<typename T>
	class IValue : public IField
	{
	public:
		template<typename ...TArgs, std::enable_if_t<std::is_constructible_v<T, TArgs...>> * = nullptr>
		explicit IValue(TArgs... i_tArgs)
			: m_tValue(std::forward<TArgs>(i_tArgs)...)
		{ }

	public:
		const T &value() const
		{
			return this->m_tValue;
		}

		T &value()
		{
			return this->m_tValue;
		}

	protected:
		T		m_tValue;
	};

	template<typename T = int, bool IsHidden = false>
	class IData : public IValue<T>
	{
	public:
		using IValue<T>::IValue;

		virtual ~IData() = default;

	public:
		bool doSerialize(CWriteStream &i_oStream)
		{
			if (!IsHidden)
			{
				return this->serialize(i_oStream);
			}

			return true;
		}

	private:
		virtual bool serialize(CWriteStream &i_oStream)
		{
			return true;
		}

	public:
		bool doDeserialize(CReadStream &i_oStream)
		{
			if (!IsHidden)
			{
				return this->deserialize(i_oStream);
			}

			return true;
		}

		bool doDeserialize(const char *i_szString)
		{
			if (!IsHidden)
			{
				return this->deserialize(i_szString);
			}

			return true;
		}

		bool doDeserialize(std::vector<std::string>::const_iterator &i_oIt, const std::vector<std::string>::const_iterator &i_oItEnd)
		{
			if (!IsHidden)
			{
				return this->deserialize(i_oIt, i_oItEnd);
			}

			return true;
		}

	private:
		virtual bool deserialize(CReadStream &i_oStream)
		{
			return true;
		}

		virtual bool deserialize(const char *i_szString)
		{
			return true;
		}

		virtual bool deserialize(std::vector<std::string>::const_iterator &i_oIt, const std::vector<std::string>::const_iterator &i_oItEnd)
		{
			++i_oIt; return true;
		}

	public:
		virtual size_t size() const
		{
			return sizeof(*this);
		}

		virtual bool isHidden() const
		{
			return IsHidden;
		}

		virtual SCompareData compareData() const
		{
			return { static_cast<const void *>(&this->m_tValue), sizeof(T) };
		}

	public:
		auto begin() const
		{
			return this->m_tValue.begin();
		}

		auto end() const
		{
			return this->m_tValue.end();
		}

	public:
		MakeAssignmentOperatorsPrivate(T);


		auto &operator[](const size_t i_nIndex)
		{
			return this->m_tValue[i_nIndex];
		}
		const auto &operator[](const size_t i_nIndex) const
		{
			return this->m_tValue[i_nIndex];
		}


		SerializerEnableIfNotArithmetic(IData<T> &)
			operator--()
		{
			--this->m_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator--(int)
		{
			--this->m_tValue;
			return *this;
		}


		SerializerEnableIfNotArithmetic(IData<T> &)
			operator++()
		{
			++this->m_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator++(int)
		{
			++this->m_tValue;
			return *this;
		}


		SerializerEnableIfNotArithmetic(IData<T> &)
			operator+=(const T &i_tValue)
		{
			this->m_tValue += i_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator-=(const T &i_tValue)
		{
			this->m_tValue -= i_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator*=(const T &i_tValue)
		{
			this->m_tValue *= i_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator/=(const T &i_tValue)
		{
			this->m_tValue /= i_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator%=(const T &i_tValue)
		{
			this->m_tValue %= i_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator&=(const T &i_tValue)
		{
			this->m_tValue &= i_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator|=(const T &i_tValue)
		{
			this->m_tValue |= i_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator^=(const T &i_tValue)
		{
			this->m_tValue ^= i_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator<<=(const T &i_tValue)
		{
			this->m_tValue <<= i_tValue;
			return *this;
		}
		SerializerEnableIfNotArithmetic(IData<T> &)
			operator>>=(const T &i_tValue)
		{
			this->m_tValue >>= i_tValue;
			return *this;
		}


		SerializerEnableIfNotArithmetic(T)
			operator+(const T &i_tValue) const
		{
			return this->m_tValue + i_tValue;
		}
		SerializerEnableIfNotArithmetic(T)
			operator-(const T &i_tValue) const
		{
			return this->m_tValue - i_tValue;
		}
		SerializerEnableIfNotArithmetic(T)
			operator*(const T &i_tValue) const
		{
			return this->m_tValue * i_tValue;
		}
		SerializerEnableIfNotArithmetic(T)
			operator/(const T &i_tValue) const
		{
			return this->m_tValue / i_tValue;
		}
		SerializerEnableIfNotArithmetic(T)
			operator%(const T &i_tValue) const
		{
			return this->m_tValue % i_tValue;
		}
		SerializerEnableIfNotArithmetic(T)
			operator&(const T &i_tValue) const
		{
			return this->m_tValue & i_tValue;
		}
		SerializerEnableIfNotArithmetic(T)
			operator|(const T &i_tValue) const
		{
			return this->m_tValue | i_tValue;
		}
		SerializerEnableIfNotArithmetic(T)
			operator^(const T &i_tValue) const
		{
			return this->m_tValue ^ i_tValue;
		}
		SerializerEnableIfNotArithmetic(T)
			operator<<(const T &i_tValue) const
		{
			return this->m_tValue << i_tValue;
		}
		SerializerEnableIfNotArithmetic(T)
			operator>>(const T &i_tValue) const
		{
			return this->m_tValue >> i_tValue;
		}


		SerializerEnableIfNotArithmetic(bool)
			operator!() const
		{
			return !this->m_tValue;
		}
		SerializerEnableIfNotArithmetic(bool)
			operator==(const T &i_tValue) const
		{
			return this->m_tValue == i_tValue;
		}
		SerializerEnableIfNotArithmetic(bool)
			operator!=(const T &i_tValue) const
		{
			return this->m_tValue != i_tValue;
		}
		SerializerEnableIfNotArithmetic(bool)
			operator>(const T &i_tValue) const
		{
			return this->m_tValue > i_tValue;
		}
		SerializerEnableIfNotArithmetic(bool)
			operator>=(const T &i_tValue) const
		{
			return this->m_tValue >= i_tValue;
		}
		SerializerEnableIfNotArithmetic(bool)
			operator<(const T &i_tValue) const
		{
			return this->m_tValue < i_tValue;
		}
		SerializerEnableIfNotArithmetic(bool)
			operator<=(const T &i_tValue) const
		{
			return this->m_tValue <= i_tValue;
		}


		template<typename T1 = T>
		typename std::enable_if<std::is_pointer<T1>::value, T>::type
			operator->() const
		{
			return this->m_tValue;
		}

		template<typename T1 = T>
		typename std::enable_if<!std::is_pointer<T1>::value, T *>::type
			operator->()
		{
			return &this->m_tValue;
		}


		T *operator&()
		{
			return &this->m_tValue;
		}
		const T *operator&() const
		{
			return &this->m_tValue;
		}


		T &operator*()
		{
			return *this->m_tValue;
		}
		const T &operator*() const
		{
			return *this->m_tValue;
		}


		operator T &()
		{
			return this->m_tValue;
		}
		operator const T &() const
		{
			return this->m_tValue;
		}
	};
#pragma pack(pop)

	template<typename T>
	std::ostream &operator<<(std::ostream &i_oStream, const IValue<T> &i_tData)
	{
		i_oStream << i_tData.value();
		return i_oStream;
	}
	template<typename T>
	std::istream &operator>>(std::istream &i_oStream, const IValue<T> &i_tData)
	{
		i_oStream >> i_tData.value();
		return i_oStream;
	}

	template<typename T>
	std::wostream &operator<<(std::wostream &i_oStream, const IValue<T> &i_tData)
	{
		i_oStream << i_tData.value();
		return i_oStream;
	}
	template<typename T>
	std::wistream &operator>>(std::wistream &i_oStream, const IValue<T> &i_tData)
	{
		i_oStream >> i_tData.value();
		return i_oStream;
	}

	// When callback return false then break
	template <typename TStructFieldType = IData<>, typename TStruct >
	bool IterateSerializableStruct(TStruct *i_pStruct, std::function<bool(TStructFieldType *)> i_pCallback, const size_t i_nStructSize = sizeof(TStruct))
	{
		for (size_t i = (std::has_virtual_destructor_v<TStruct> ? sizeof(void *) : 0); i < i_nStructSize;)
		{
			TStructFieldType *pData = SerializableGetField(TStructFieldType, i_pStruct, i);

			if (!i_pCallback(pData))
			{
				return false;
			}

			i += pData->size();
		}

		return true;
	}
	
	template <typename TStructFieldType = IData<>, typename TStruct >
	bool IterateSerializableStruct(TStruct &i_oStruct, std::function<bool(TStructFieldType *)> i_pCallback, const size_t i_nStructSize = sizeof(TStruct))
	{
		return IterateSerializableStruct<TStructFieldType, TStruct>(&i_oStruct, i_pCallback, i_nStructSize);
	}


	template <typename TStructFieldType = IData<>, typename TStruct >
	std::vector<size_t> GetSerializableFieldOffsets(TStruct *i_pStruct, const bool i_fIncludeHidden = true, const size_t i_nStructSize = sizeof(TStruct))
	{
		std::vector<size_t> anOffsets;
		anOffsets.reserve(10);

		for (size_t i = (std::has_virtual_destructor_v<TStruct> ? sizeof(void *) : 0); i < i_nStructSize;)
		{
			TStructFieldType *pData = SerializableGetField(TStructFieldType, i_pStruct, i);

			if (i_fIncludeHidden || !pData->isHidden())
			{
				anOffsets.push_back(i);
			}

			i += pData->size();
		}

		return anOffsets;
	}
	
	template <typename TStructFieldType = IData<>, typename TStruct >
	std::vector<size_t> GetSerializableFieldOffsets(TStruct &i_oStruct, const bool i_fIncludeHidden = true, const size_t i_nStructSize = sizeof(TStruct))
	{
		return GetSerializableFieldOffsets<TStructFieldType, TStruct>(&i_oStruct, i_fIncludeHidden, i_nStructSize);
	}


	template <typename TStructFieldType = IData<>, typename TStruct>
	TStructFieldType *GetSerializableFieldByIndex(TStruct *i_pStruct, const size_t i_nIndex)
	{
		TStructFieldType *pReturn = nullptr;
		size_t nCurrentIndex = 0;
		IterateSerializableStruct<TStructFieldType>(i_pStruct, [&](TStructFieldType *i_pData)
		{
			if (nCurrentIndex == i_nIndex)
			{
				pReturn = i_pData;
				return false;
			}

			nCurrentIndex++;

			return true;
		});

		return pReturn;
	}

	template <typename TStructFieldType = IData<>, typename TStruct>
	TStructFieldType *GetSerializableFieldByIndex(TStruct &i_oStruct, const size_t i_nIndex)
	{
		return GetSerializableFieldByIndex<TStructFieldType, TStruct>(&i_oStruct, i_nIndex);
	}


	template <typename TStructFieldType = IData<>, typename TStruct>
	bool CompareSerializableStruct(TStruct *i_pStruct, TStruct *i_pCompareStruct, const bool i_fIncludeHidden = false)
	{
		std::vector<size_t> anOffsets = GetSerializableFieldOffsets(i_pStruct, i_fIncludeHidden);

		for (size_t nOffset : anOffsets)
		{
			TStructFieldType *pFirst = SerializableGetField(TStructFieldType, i_pStruct, nOffset);
			TStructFieldType *pSecond = SerializableGetField(TStructFieldType, i_pCompareStruct, nOffset);

			const SCompareData oFirstCompareData = pFirst->compareData();
			const SCompareData oSecondCompareData = pSecond->compareData();

			if (oFirstCompareData.nSize != oSecondCompareData.nSize)
			{
				return false;
			}

			if (memcmp(oFirstCompareData.pData, oSecondCompareData.pData, oFirstCompareData.nSize) != 0)
			{
				return false;
			}
		}

		return true;
	}

	template <typename TStructFieldType = IData<>, typename TStruct>
	bool CompareSerializableStruct(TStruct &i_oStruct, TStruct &i_oCompareStruct, const bool i_fIncludeHidden = false)
	{
		return CompareSerializableStruct<TStructFieldType, TStruct>(&i_oStruct, &i_oCompareStruct, i_fIncludeHidden);
	}
}