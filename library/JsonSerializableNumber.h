#pragma once

#include "SerializableArithmetic.h"
#include "JsonSerializableType.h"
#include "SerializableBool.h"
#include "Global.h"

namespace Json
{
	namespace Serialize
	{
#pragma pack(push, 1)

		template <typename T>
		class IJsonNumber : public IJsonType<T, ::Serialize::IArithmetic<T>>
		{
		public:
			IJsonNumber(const std::string_view i_stName) : IJsonType<T, ::Serialize::IArithmetic<T>>(i_stName) {}
		private:
			bool onDeserialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{
				CObject *pObject = this->getObject(i_oObject, i_bIsStrict);
				
				if (!pObject)
				{
					return true;
				}
				else if (pObject->type() == EJsonType::JTNumber)
				{
					if (std::is_same_v<T, float>)
					{
						 this->operator=(static_cast<T>(pObject->toFloat()));
					}
					else if (std::is_same_v<T, double>)
					{
						this->operator=(static_cast<T>(pObject->toDouble()));
					}
					else if (sizeof(T) == 8)
					{
						 this->operator=(static_cast<T>(pObject->toUInt64()));
					}
					else
					{
						this->operator=(static_cast<T>(pObject->toInt()));
					}	
				}
				else
				{
					throw CDifferentTypesException();
				}

				return true;
			}

			bool onSerialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{
				return this->serialize(i_oObject, i_bIsStrict);
			}
		public:
			using  IJsonType<T, ::Serialize::IArithmetic<T>>::IJsonType;
			auto &operator=(const T &i_tValue)
			{
				this->m_tValue = i_tValue;
				this->setNull(false);
				return *this;
			}

			auto &operator=(T &&i_tValue)
			{
				this->m_tValue = i_tValue;
				this->setNull(false);
				return *this;
			}
		};
		
		typedef IJsonNumber<char> IJsonChar;
		typedef IJsonNumber<byte> IJsonByte;
		typedef IJsonNumber<short> IJsonShort;
		typedef IJsonNumber<ushort> IJsonUShort;
		typedef IJsonNumber<int> IJsonInt;
		typedef IJsonNumber<uint> IJsonUInt;
		typedef IJsonNumber<int64> IJsonInt64;
		typedef IJsonNumber<uint64> IJsonUInt64;
		typedef IJsonNumber<float> IJsonFloat;
		typedef IJsonNumber<double> IJsonDouble;

#pragma pack(pop)
	}
}