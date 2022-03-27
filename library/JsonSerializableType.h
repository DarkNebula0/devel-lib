#pragma once

#include "SerializableData.h"
#include "SerializableBool.h"
#include "JsonDefines.h"
#include "JsonObject.h"

#include <string_view>

namespace Json
{
	namespace Serialize
	{
#pragma pack(push, 1)
		template<typename T = int, typename TData = ::Serialize::IData<T>>
		class IJsonType : public TData
		{
		protected:
			IJsonType(const std::string_view i_stName): m_stName(i_stName), m_bIsNull(false) {}
			virtual ~IJsonType() = default;
		private:
			::Serialize::IData<std::string_view, true> m_stName;
			::Serialize::IBool<true> m_bIsNull;
		protected:
			virtual bool onDeserialize(CObject &i_oObject, const bool i_bIsStrict = false)
			{
				return true;
			};

			virtual bool onSerialize(CObject &i_oObject, const bool i_bIsStrict = false)
			{
				return true;
			}
		protected:
			bool serialize(CObject &i_oObject, const bool i_bIsStrict = false)
			{
				if (checkSerializeObject(i_oObject, i_bIsStrict))
				{
					return true;
				}

				if (std::is_arithmetic_v<T>)
				{
					i_oObject.get(this->name()) = static_cast<T>(this->m_tValue);
				}
				else
				{
					i_oObject.get(this->name()) = this->m_tValue;
				}
				return true;
			}

			std::string_view name() const
			{
				return this->m_stName;
			}

			bool isNull() const {
				return this->m_bIsNull;
			}

			void setNull(bool i_bState) {
				this->m_bIsNull = i_bState;
			}

			CObject *getObject(CObject &i_oObject, const bool i_bIsStrict) {
				CObject *pObject = (i_oObject.type() != EJsonType::JTArray) ? i_oObject.find(this->name())
					: ((this->name().empty()) ? &i_oObject : i_oObject.find(this->name()));

				if (!pObject)
				{
					if (i_bIsStrict)
					{
						throw CInvalidData();
					}
					IJsonType::setNull(true);
				}
				else if (pObject->type() == EJsonType::JTNull)
				{
					IJsonType::setNull(true);
					pObject = nullptr;
				}
				return pObject;
			}

			bool checkSerializeObject(CObject &i_oObject, const bool i_bIsStrict) {
				if (!i_bIsStrict && this->isNull())
				{
					return true;
				}
				if (this->name().empty())
				{
					throw CInvalidObjectName();
				}

				if (this->isNull()) {
					i_oObject.get(this->name()).setNull();
				}
			}
		public:
			bool doDeserialize(CObject &i_oObject, const bool i_bIsStrict = false)
			{
				return this->onDeserialize(i_oObject, i_bIsStrict);
			}

			bool doSerialize(CObject &i_oObject, const bool i_bIsStrict = false)
			{
				return this->onSerialize(i_oObject, i_bIsStrict);
			}
		public:
			size_t size() const override
			{
				return sizeof(*this);
			}
		public:
			using TData::TData;
			CreateSerializeOperatorsCustomDerive(T);
		};
#pragma pack(pop)

		template<typename TField, typename TStruct>
		TField *GetJsonFieldByName(TStruct &i_oStruct, const std::string_view i_sName)
		{
			TField *pField = nullptr;
			::Serialize::IterateSerializableStruct<TField>(&i_oStruct, [&](TField *i_pData)
			{
				if (i_pData->name() == i_sName)
				{
					pField = i_pData;
					return false;
				}

				return true;
			});

			return pField;
		}
	}
}