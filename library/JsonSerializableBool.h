#pragma once

#include "JsonSerializableType.h"
#include "SerializableBool.h"
#include "Global.h"

namespace Json
{
	namespace Serialize
	{
#pragma pack(push, 1)
		class IJsonBool : public IJsonType<bool,::Serialize::IBool<>>
		{
		public:
			IJsonBool(const std::string_view i_stName) : IJsonType<bool, ::Serialize::IBool<>>(i_stName) {}
		private:
			bool onDeserialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{
				CObject *pObject = this->getObject(i_oObject, i_bIsStrict);

				if (!pObject)
				{
					return true;
				}
				else if (pObject->type() != EJsonType::JTBoolean)
				{
					throw CDifferentTypesException();
				}

				this->operator=(
					pObject->empty()		||
					pObject->at(0) < '1'	||
					pObject->at(0) == 'f'	||
					pObject->at(0) == 'F'	? false : true);

				return true;
			}
			
			bool onSerialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{
				return this->serialize(i_oObject, i_bIsStrict);
			}

		public:
			using  IJsonType<bool, ::Serialize::IBool<>>::IJsonType;
			IJsonBool &operator=(const bool &i_tValue) {
				this->setNull(false);
				this->m_tValue = i_tValue;
				return *this;
			}
			IJsonBool &operator=(bool &&i_tValue) {
				this->setNull(false);
				this->m_tValue = std::move(i_tValue);
				return *this;
			}
		};
#pragma pack(pop)
	}
}