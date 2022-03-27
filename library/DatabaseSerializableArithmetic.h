#pragma once
#include "SerializableArithmetic.h"

#include "DatabaseSerializable.h"


namespace MySQL
{
	namespace ORM
	{
		namespace Serialize
		{
#pragma pack(push, 1)
			template<size_t ColumnIndex, typename T, bool IsHidden = false>
			class IArithmetic : public IData<ColumnIndex, T, IsHidden, ::Serialize::IArithmetic<T, IsHidden>>
			{
			public:
				void bind(MySQL::SPreparedBind *i_pBind, const size_t i_nIndex, const MySQL::EPreparedBind i_eBindType) override
				{
#define USE_BIND(unsignedName, signedName)	if (std::is_unsigned_v<T>)								\
											{ unsignedName(i_pBind, i_nIndex, this->m_tValue); }	\
											else													\
											{ signedName(i_pBind, i_nIndex, this->m_tValue); }		\
											break;

					switch (sizeof(T))
					{
					case 1:
						USE_BIND(SetMySQLBindByte, SetMySQLBindChar);
					case 2:
						USE_BIND(SetMySQLBindUShort, SetMySQLBindShort);
					case 4:
						USE_BIND(SetMySQLBindUInt, SetMySQLBindInt);
					case 8:
						USE_BIND(SetMySQLBindUInt64, SetMySQLBindInt64);
					}
				}

			public:
				using IData<ColumnIndex, T, IsHidden, ::Serialize::IArithmetic<T, IsHidden>>::IData;
				CreateSerializeOperatorsCustomDerive(T);
			};

			template<size_t ColumnIndex = -1, bool IsHidden = false>
			using IChar = IArithmetic<ColumnIndex, char, IsHidden>;

			template<size_t ColumnIndex = -1, bool IsHidden = false>
			using IByte = IArithmetic<ColumnIndex, byte, IsHidden>;

			template<size_t ColumnIndex = -1, bool IsHidden = false>
			using IShort = IArithmetic<ColumnIndex, short, IsHidden>;

			template<size_t ColumnIndex = -1, bool IsHidden = false>
			using IUShort = IArithmetic<ColumnIndex, ushort, IsHidden>;

			template<size_t ColumnIndex = -1, bool IsHidden = false>
			using IInt = IArithmetic<ColumnIndex, int, IsHidden>;

			template<size_t ColumnIndex = -1, bool IsHidden = false>
			using IUInt = IArithmetic<ColumnIndex, uint, IsHidden>;

			template<size_t ColumnIndex = -1, bool IsHidden = false>
			using IInt64 = IArithmetic<ColumnIndex, int64, IsHidden>;

			template<size_t ColumnIndex = -1, bool IsHidden = false>
			using IUInt64 = IArithmetic<ColumnIndex, uint64, IsHidden>;
#pragma pack(pop)
		}
	}
}