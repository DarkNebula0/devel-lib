#pragma once
#include <memory>

#include "DatabaseSerializable.h"

namespace MySQL
{
	namespace ORM
	{
		namespace Serialize
		{
#pragma pack(push, 1)
			template <typename T = std::weak_ptr<ORM::CTable<>>>
			class ITable : public IData<-1, T, true>
			{
			public:
				void bind(MySQL::SPreparedBind *i_pBind, const size_t i_nIndex, const MySQL::EPreparedBind i_eBindType) override
				{
					throw CShouldNotExecuteException();
				}

			public:
				using IData<-1, T, true>::IData;
				CreateSerializeOperatorsCustomDerive(T);
			};
#pragma pack(pop)
		}
	}
}