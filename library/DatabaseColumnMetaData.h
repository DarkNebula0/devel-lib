#pragma once
#include <vector>

#include "Typedef.h"

namespace MySQL
{
	namespace ORM
	{
		enum EColumnMetaDataIndex : byte
		{
			CMDColumnName,
			CMDOrdinalPosition,
			CMDIsNullable,
			CMDDataType,
			CMDCharacterMaximumLength,
			CMDNumericPrecision,
			CMDNumericScale,
			CMDColumnType,
			CMDColumnKey,
			CMDExtra,

			CMDIgnore,
		};

		enum EDataType
		{
			DTChar,
			DTVarChar,

			DTTinyText,
			DTText,
			DTMediumText,
			DTLongText,

			DTBinary,
			DTVarBinary,

			DTBit,

			DTTinyInt,
			DTSmallInt,
			DTMediumInt,
			DTInt,
			DTInteger,
			DTBigInt,

			DTDecimal,
			DTNumeric,
			DTFloat,
			DTDouble,
			DTReal,

			DTDate,
			DTTime,
			DTDateTime,
			DTTimestamp,
			DTYear,

			DTTinyBlob,
			DTBlob,
			DTMediumBlob,
			DTLongBlob,

			DTSet,
			DTEnum,

			DTGeometry,
			DTGeometryCollection,
		};

		struct SDataType
		{
			EDataType eDataType;
			EColumnMetaDataIndex nLengthIndex; // Which length? (CHARACTER_MAXIMUM_LENGTH, NUMBER_PRECISION)

			bool fParseValidValues;
		};

		struct SDataTypeWrapper
		{
			const SDataType *pDataType;
			EDataType eRealType;

			SDataTypeWrapper(const SDataType *i_pDataType, const EDataType i_eRealType)
				: pDataType(i_pDataType), eRealType(i_eRealType)
			{}
		};


		struct SColumnMetaData
		{
			std::string sName;
			std::shared_ptr<SDataTypeWrapper> pDataTypeWrapper;

			size_t nPosition;
			size_t nLength;    // NUMERIC_PRECISION
			size_t nPrecision; // NUMERIC_SCALE

			bool fIsNullable;
			bool fIsUnsigned;
			bool fIsPrimaryKey;
			bool fIsAutoIncrement;
			bool fIsVirtual;

			// Enum | Set
			std::vector<std::string> asValidValues;
		};
	}
}