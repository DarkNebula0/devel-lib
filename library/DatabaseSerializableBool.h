#pragma once
#include "DatabaseSerializable.h"
#include "SerializableBool.h"

namespace MySQL
{
	namespace ORM
	{
		namespace Serialize
		{
#pragma pack(push, 1)
			template <size_t ColumnIndex = -1, bool IsHidden = false, typename T = bool>
			class IBoolean : public IData<ColumnIndex, T, IsHidden, ::Serialize::IBool<IsHidden, T>>
			{
			public:
				void bind(MySQL::SPreparedBind *i_pBind, const size_t i_nIndex, const MySQL::EPreparedBind i_eBindType) override
				{
					SetMySQLBindByte(i_pBind, i_nIndex, this->m_tValue);
				}

			public:
				using IData<ColumnIndex, T, IsHidden, ::Serialize::IBool<IsHidden, T>>::IData;
				CreateSerializeOperatorsCustomDerive(T);
			};
#pragma pack(pop)
		}
	}
}