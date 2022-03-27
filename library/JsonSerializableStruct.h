#pragma once

#include "JsonSerializableType.h"
#include "SerializableData.h"
#include "JsonSerializer.h"
#include "Global.h"

namespace Json
{
	namespace Serialize
	{
#pragma pack(push, 1)
		template <typename TStruct>
		class IJsonStruct : public IJsonType<TStruct, ::Serialize::IData<TStruct>>
		{
		public:
			IJsonStruct(const std::string_view i_stName) : IJsonType<TStruct, ::Serialize::IData<TStruct>>(i_stName) {}
		private:
			bool onDeserialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{	
				CObject *pObject = this->getObject(i_oObject, i_bIsStrict);

				if (!pObject)
				{
					return true;
				}
				if (pObject->type() != EJsonType::JTObject)
				{
					throw CDifferentTypesException();
				}

				return Serialize::DeserializeObject(this->m_tValue, *pObject);
			}

			bool onSerialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{
				if (this->checkSerializeObject(i_oObject, i_bIsStrict))
				{
					return true;
				}

				return Serialize::SerializeObject(this->m_tValue, i_oObject.get(this->name()), i_bIsStrict);
			}
		public:
			using IJsonType<TStruct, ::Serialize::IData<TStruct>>::IJsonType;
			IJsonStruct &operator=(const TStruct &i_tValue)
			{
				this->m_tValue = i_tValue;
				this->setNull(false);
				return *this;
			}

			IJsonStruct &operator=(TStruct &&i_tValue)
			{
				this->m_tValue = std::move(i_tValue);
				this->setNull(false);
				return *this;
			}
		};
#pragma pack(pop)
	}
}
