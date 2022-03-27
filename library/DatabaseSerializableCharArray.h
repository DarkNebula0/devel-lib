#pragma once
#include "SerializableCharArray.h"
#include "DatabaseSerializable.h"

namespace MySQL
{
	namespace ORM
	{
		namespace Serialize
		{
#pragma pack(push, 1)
			template<size_t Size, size_t ColumnIndex = -1, bool IsHidden = false, typename T = CCharArray<Size>>
			class ICharArray : public IData<ColumnIndex, T, IsHidden, ::Serialize::ICharArray<Size, IsHidden, T>>
			{
			public:
				void bind(MySQL::SPreparedBind *i_pBind, const size_t i_nIndex, const MySQL::EPreparedBind i_eBindType) override
				{
					if (i_eBindType == MySQL::EPreparedBind::PBResult)
					{
						SetMySQLBindCharArray(i_pBind, i_nIndex, this->m_tValue, true);
					}
					else if (i_eBindType == MySQL::EPreparedBind::PBParam)
					{
						SetMySQLBindCharArray(i_pBind, i_nIndex, this->m_tValue, false);
					}
				}

			public:
				using IData<ColumnIndex, T, IsHidden, ::Serialize::ICharArray<Size, IsHidden, T>>::IData;
				CreateSerializeOperatorsCustomDerive(T);
			};
#pragma pack(pop)
		}
	}
}