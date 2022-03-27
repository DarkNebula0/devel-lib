#pragma once
#include "SerializableData.h"

namespace Serialize
{
#pragma pack(push, 1)
	template<bool IsHidden = false, typename T = std::wstring>
	class IWString : public IData<T, IsHidden>
	{
	public:
		SCompareData compareData() const override
		{
			return { static_cast<const void *>(this->m_tValue.c_str()), this->m_tValue.size() * 2 };
		}

	private:
		bool serialize(CWriteStream &i_oStream) override
		{
			i_oStream.push<uint>(static_cast<uint>(this->m_tValue.size()));
			i_oStream.push(this->m_tValue, false);
			return true;
		}

	private:
		bool deserialize(CReadStream &i_oStream) override
		{
			const size_t nSize = static_cast<size_t>(i_oStream.get<uint>());
			this->m_tValue = i_oStream.getWString(nSize);
			return true;
		}

		bool deserialize(const char *i_szString) override
		{
			this->m_tValue = std::wstring(i_szString, i_szString + strlen(i_szString));
			return true;
		}

		bool deserialize(std::vector<std::string>::const_iterator &i_oIt, const std::vector<std::string>::const_iterator &i_oItEnd) override
		{
			this->m_tValue = std::wstring(i_oIt->begin(), i_oIt->end());
			return true;
		}

	public:
		CreateSerializeOperators(T, IsHidden);
	};
#pragma pack(pop)
}