#pragma once
#include "Serializer.h"

namespace Serialize
{
	template<typename T>
	bool SerializeStream(T &i_oStruct, CWriteStream &i_oStream)
	{
		return IterateSerializableStruct<IData<>>(&i_oStruct, [&](IData<> *i_pData)
		{
			if (!i_pData->doSerialize(i_oStream))
			{
				return false;
			}

			return true;
		});
	}

	template<typename T>
	bool DeserializeStream(T &i_oStruct, CReadStream &i_oStream, size_t i_nStructSize = sizeof(T))
	{
		if (i_oStream.buffer())
		{
			return IterateSerializableStruct<IData<>>(&i_oStruct, [&](IData<> *i_pData)
			{
				if (!i_pData->isHidden())
				{
					if (i_oStream.isEoB())
					{
						throw CTooLessDataInPacketException();
					}

					if (!i_pData->doDeserialize(i_oStream))
					{
						return false;
					}
				}

				return true;
			}, i_nStructSize);
		}

		throw CNoBufferException();
	}
}