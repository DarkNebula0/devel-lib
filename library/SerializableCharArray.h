#pragma once
#include "SerializableData.h"
#include "CharArray.h"

namespace Serialize
{
#pragma pack(push, 1)
	template<size_t Size, bool IsHidden = false, typename T = CCharArray<Size>>
	class ICharArray : public IData<T, IsHidden>
	{
	private:
		bool serialize(CWriteStream &i_oStream) override
		{
			i_oStream.push<Size>(this->m_tValue);
			return true;
		}

	private:
		bool deserialize(CReadStream &i_oStream) override
		{
			this->m_tValue = i_oStream.getArray<Size>();
			return true;
		}

		bool deserialize(const char *i_szValue) override
		{
			this->m_tValue = i_szValue;
			return true;
		}

		bool deserialize(std::vector<std::string>::const_iterator &i_oIt, const std::vector<std::string>::const_iterator &i_oItEnd) override
		{
			this->m_tValue = *i_oIt++;
			return true;
		}

	public:
		CreateSerializeOperators(T, IsHidden);
	};
#pragma pack(pop)
}