#pragma once

#include "JsonSerializableType.h"
#include "JsonDocument.h"
#include "JsonObject.h"

namespace Json
{
	namespace Serialize
	{
		template<typename T>
		bool DeserializeObject(T &i_oStruct, CObject &i_oObject, const bool i_bIsStrict = false, size_t i_nStructSize = sizeof(T))
		{
			if (i_oObject.type() == EJsonType::JTObject || i_oObject.type() == EJsonType::JTArray)
			{
				return ::Serialize::IterateSerializableStruct<IJsonType<>>(&i_oStruct, [&](IJsonType<> *i_pData)
				{
					return i_pData->doDeserialize(i_oObject, i_bIsStrict);
				}, i_nStructSize);
			}

			throw CDifferentTypesException();
		}

		template<typename T>
		bool DeserializeDocument(T &i_oStruct, CDocument &i_oDocument, const bool i_bIsStrict = false, size_t i_nStructSize = sizeof(T))
		{
			return ::Serialize::IterateSerializableStruct<IJsonType<>>(&i_oStruct, [&](IJsonType<> *i_pData)
			{
				return i_pData->doDeserialize(i_oDocument.toObject(), i_bIsStrict);
			}, i_nStructSize);
		}


		template<typename T>
		bool SerializeObject(T &i_oStruct, CObject &i_oObject, const bool i_bIsStrict = false, size_t i_nStructSize = sizeof(T))
		{
			return ::Serialize::IterateSerializableStruct<IJsonType<>>(&i_oStruct, [&](IJsonType<> *i_pData)
			{
				return i_pData->doSerialize(i_oObject, i_bIsStrict);
			}, i_nStructSize);
		}

		template<typename T>
		bool SerializeDocument(T &i_oStruct, CDocument &i_oDocument, const bool i_bIsStrict = false, size_t i_nStructSize = sizeof(T))
		{
			i_oDocument.clear();
			return SerializeObject(i_oStruct, i_oDocument.toObject(), i_bIsStrict, i_nStructSize);
		}

	}
}