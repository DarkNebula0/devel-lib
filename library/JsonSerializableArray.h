#pragma once

#include "JsonSerializableType.h"
#include "SerializableVector.h"
#include "JsonSerializer.h"
#include "JsonArray.h"
#include "Global.h"

namespace Json
{
	namespace Serialize
	{
#pragma pack(push, 1)
		template <typename TStruct>
		class IJsonArray : public IJsonType<TStruct, ::Serialize::IData<std::vector<TStruct>>>
		{
		public:
			IJsonArray(const std::string_view i_stName) : IJsonType<TStruct, ::Serialize::IData<std::vector<TStruct>>>(i_stName) {};
		private:
			bool onDeserialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{
				CObject *pObject = this->getObject(i_oObject, i_bIsStrict);
				if (!pObject)
				{
					return true;
				}
				else if (pObject->type() != EJsonType::JTArray)
				{
					throw CDifferentTypesException();
				}

				this->setNull(false);
				CArray oArray = pObject->toArray();
				for (const CObject &oItem : oArray)
				{
					TStruct grStruct;
					if (!Serialize::DeserializeObject(grStruct, const_cast<CObject &>(oItem)))
					{
						return false;
					}

					this->m_tValue.push_back(std::move(grStruct));
				}

				return true;
			}

			bool onSerialize(CObject &i_oObject, const bool i_bIsStrict = false) override
			{
				if (this->checkSerializeObject(i_oObject, i_bIsStrict))
				{
					return true;
				}

				CArray oArray;
				for (TStruct &grItem : this->m_tValue)
				{
					CObject oObject;
					if (!Serialize::SerializeObject(grItem, oObject))
					{
						return false;
					}
					oArray.push_back(oObject);
				}

				if (this->name().empty())
				{
					i_oObject = oArray;
				}
				else
				{
					i_oObject.get(this->name()) = oArray;
				}
				return true;
			}
		public:
			using IJsonType<TStruct, ::Serialize::IData<std::vector<TStruct>>>::IJsonType;

			auto &operator=(const IJsonArray<TStruct> &i_tValue)	
			{														
				this->setNull(false);
				this->m_tValue = i_tValue;
				return *this;										
			}														
																	
			auto &operator=(IJsonArray<TStruct> &&i_tValue)			
			{														
				this->setNull(false);
				this->m_tValue = i_tValue;
				return *this;										
			}
		};

#pragma pack(pop)
	}
}