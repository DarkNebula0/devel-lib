#pragma once

#include "JsonSerializableType.h"
#include "SerializableString.h"
#include "Global.h"

namespace Json
{
	namespace Serialize
	{
#pragma pack(push, 1)
		class IJsonString : public IJsonType<std::string, ::Serialize::IString<>>
		{
		public:
			IJsonString(const std::string_view i_stName): IJsonType<std::string, ::Serialize::IString<>>(i_stName) {}
		private:
			bool onDeserialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{
				CObject *pObject = this->getObject(i_oObject, i_bIsStrict);

				if (!pObject)
				{
					return true;
				}
				else if (pObject->type() != EJsonType::JTString)
				{
					throw CDifferentTypesException();
				}

				this->operator=(*pObject);
				return true;
			}

			bool onSerialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{
				return this->serialize(i_oObject, i_bIsStrict);
			}

		public:
			using IJsonType<std::string, ::Serialize::IString<>>::IJsonType;
			IJsonString &operator=(const std::string &i_tValue) {
				this->setNull(false);
				this->m_tValue = i_tValue;
				return *this;
			}
			IJsonString &operator=(std::string &&i_tValue) {
				this->setNull(false);
				this->m_tValue = std::move(i_tValue);
				return *this;
			};
		};
#pragma pack(pop)
	}
}