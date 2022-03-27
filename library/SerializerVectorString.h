#pragma once
#include "Serializer.h"

namespace Serialize
{
	template <typename T>
	bool DeserializeVectorString(T &i_oStruct, const std::vector<std::string> &i_asPacket)
	{
		auto oIt = i_asPacket.begin();
		const auto oItEnd = i_asPacket.end();

		return IterateSerializableStruct<IData<>>(&i_oStruct, [&](IData<> *i_pData)
		{
			if (oIt == i_asPacket.end())
			{
				throw CTooLessDataInPacketException();
			}

			if (!i_pData->doDeserialize(oIt, oItEnd))
			{
				return false;
			}

			return true;
		});
	}
}