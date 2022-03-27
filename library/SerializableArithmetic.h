#pragma once
#include "Typedef.h"
#include "Global.h"

#include "SerializableData.h"

namespace Serialize
{
#pragma pack(push, 1)
	template<typename T, bool IsHidden = false, typename std::enable_if<std::is_arithmetic<T>::value> * = nullptr>
	class IArithmetic : public IData<T, IsHidden>
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

		bool deserialize(const char *i_szValue) override
		{
			if (sizeof(T) <= 4)
			{
				this->m_tValue = static_cast<T>(Utils::StringToUInt(i_szValue));
			}
			else
			{
				this->m_tValue = static_cast<T>(Utils::StringToUInt64(i_szValue));
			}

			return true;
		}

		bool deserialize(std::vector<std::string>::const_iterator &i_oIt, const std::vector<std::string>::const_iterator &i_oItEnd) override
		{
			if (sizeof(T) <= 4)
			{
				this->m_tValue = static_cast<T>(Utils::StringToUInt(*i_oIt));
			}
			else
			{
				this->m_tValue = static_cast<T>(Utils::StringToUInt64(*i_oIt));
			}

			++i_oIt;
			return true;
		}

	public:
		CreateSerializeOperators(T, IsHidden);
	};

	template<bool IsHidden = false>
	using IChar = IArithmetic<char, IsHidden>;

	template<bool IsHidden = false>
	using IByte = IArithmetic<byte, IsHidden>;

	template<bool IsHidden = false>
	using IShort = IArithmetic<short, IsHidden>;

	template<bool IsHidden = false>
	using IUShort = IArithmetic<ushort, IsHidden>;

	template<bool IsHidden = false>
	using IInt = IArithmetic<int, IsHidden>;

	template<bool IsHidden = false>
	using IUInt = IArithmetic<uint, IsHidden>;

	template<bool IsHidden = false>
	using IInt64 = IArithmetic<int64, IsHidden>;

	template<bool IsHidden = false>
	using IUInt64 = IArithmetic<uint64, IsHidden>;

	template<typename T, bool IsHidden = false>
	using IEnum = Serialize::IArithmetic<T, IsHidden>;
#pragma pack(pop)
}