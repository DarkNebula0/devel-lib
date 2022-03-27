#pragma once
#include "MySQLPreparedBind.h"
#include "SerializableData.h"
#include "DatabaseORMGlobal.h"
#include "Typedef.h"
#include "Global.h"

namespace MySQL
{
	namespace ORM
	{
		namespace Serialize
		{
#pragma pack(push, 1)
			// ColumnIndex starts at 0!!! (When -1 then it doesn't parse from DB)
			template<size_t ColumnIndex = -1, typename T = int, bool IsHidden = false, typename TData = ::Serialize::IData<T, IsHidden>>
			class IData : public TData
			{
			public:
				virtual size_t columnIndex() const
				{
					return ColumnIndex;
				}

				virtual bool parseFromDatabase() const
				{
					return ColumnIndex != -1;
				}

				virtual void bind(MySQL::SPreparedBind *i_pBind, const size_t i_nIndex, const MySQL::EPreparedBind i_eBindType) = 0;

			public:
				using TData::TData;
				CreateSerializeOperatorsCustomDerive(T);
			};
#pragma pack(pop)
		}
	}
}