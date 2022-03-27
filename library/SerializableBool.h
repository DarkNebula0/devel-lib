#pragma once
#include "SerializableData.h"

namespace Serialize
{
#pragma pack(push, 1)
	template<bool IsHidden = false, typename T = bool>
	class IBool : public IData<T, IsHidden>
	{
	private:
		bool serialize(CWriteStream &i_oStream) override
		{
			i_oStream.push(this->m_tValue);
			return true;
		}

	private:
		bool deserialize(CReadStream &i_oStream) override
		{
			this->m_tValue = i_oStream.get<T>();
			return true;
		}

		bool deserialize(const char *i_szString) override
		{
			this->m_tValue = (Utils::StringToInt64(i_szString) > 0);
			return true;
		}

		bool deserialize(std::vector<std::string>::const_iterator &i_oIt, const std::vector<std::string>::const_iterator &i_oItEnd) override
		{
			this->m_tValue = static_cast<T>(Utils::StringToInt(*i_oIt));

			++i_oIt;
			return true;
		}

	public:
		CreateSerializeOperators(T, IsHidden);
	};
#pragma pack(pop)
}