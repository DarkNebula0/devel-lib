#include "DatabaseDataTypeWrapper.h"

MySQL::ORM::CColumnDataTypeWrapper *ColumnDataTypeWrapperInstance = MySQL::ORM::CColumnDataTypeWrapper::instance();

MySQL::ORM::CColumnDataTypeWrapper::CColumnDataTypeWrapper()
{
	#pragma region Char, VarChar
	{
		SDataType *pDataType   = new SDataType();
		pDataType->eDataType         = MySQL::ORM::EDataType::DTChar;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDNumericPrecision;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["char"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTChar
			);
		this->m_mapDataTypeWrappers["varchar"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTVarChar
			);
	}
	#pragma endregion
	#pragma region TinyText, Text, MediumText, LongText
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTText;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDCharacterMaximumLength;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["tinytext"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTTinyText
			);
		this->m_mapDataTypeWrappers["text"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTText
			);
		this->m_mapDataTypeWrappers["mediumtext"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTMediumText
			);
		this->m_mapDataTypeWrappers["longtext"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTLongText
			);
	}
	#pragma endregion
	#pragma region Binary
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTBinary;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDNumericPrecision;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["binary"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTBinary
			);
	}
	#pragma endregion
	#pragma region Bit
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTBit;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDNumericPrecision;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["bit"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTBit
			);
	}
	#pragma endregion
	#pragma region TinyInt, SmallInt, MediumInt, Int, Integer, BigInt
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTInt;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDNumericPrecision;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["tinyint"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTTinyInt
			);
		this->m_mapDataTypeWrappers["smallint"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTSmallInt
			);
		this->m_mapDataTypeWrappers["mediumint"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTMediumInt
			);
		this->m_mapDataTypeWrappers["int"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTInt
			);
		this->m_mapDataTypeWrappers["integer"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTInteger
			);
		this->m_mapDataTypeWrappers["bigint"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTBigInt
			);
	}
	#pragma endregion
	#pragma region Decimal, Numeric, Float, Double, Real
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTDecimal;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDNumericPrecision;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["decimal"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTDecimal
			);
		this->m_mapDataTypeWrappers["numeric"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTNumeric
			);
		this->m_mapDataTypeWrappers["float"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTFloat
			);
		this->m_mapDataTypeWrappers["double"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTDouble
			);
		this->m_mapDataTypeWrappers["real"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTReal
			);
	}
	#pragma endregion
	#pragma region Date
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTDate;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDIgnore;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["date"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTDate
			);
	}
	#pragma endregion
	#pragma region Time
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTTime;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDIgnore;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["time"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTTime
			);
	}
	#pragma endregion
	#pragma region DateTime
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTDateTime;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDIgnore;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["datetime"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTDateTime
			);
	}
	#pragma endregion
	#pragma region Timestamp
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTTimestamp;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDIgnore;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["timestamp"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTTimestamp
			);
	}
	#pragma endregion
	#pragma region Year
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTYear;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDIgnore;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["year"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTYear
			);
	}
	#pragma endregion
	#pragma region TinyBlob, Blob, MediumBlob, LongBlob
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTBlob;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDCharacterMaximumLength;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["tinyblob"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTTinyBlob
			);
		this->m_mapDataTypeWrappers["blob"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTBlob
			);
		this->m_mapDataTypeWrappers["mediumblob"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTMediumBlob
			);
		this->m_mapDataTypeWrappers["longblob"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTLongBlob
			);
	}
	#pragma endregion
	#pragma region Set
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTSet;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDCharacterMaximumLength;
		pDataType->fParseValidValues = true;

		this->m_mapDataTypeWrappers["set"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTSet
			);
	}
	#pragma endregion
	#pragma region Enum
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTEnum;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDCharacterMaximumLength;
		pDataType->fParseValidValues = true;

		this->m_mapDataTypeWrappers["enum"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTEnum
			);
	}
	#pragma endregion
	#pragma region Geometry, GeometryCollection
	{
		SDataType *pDataType = new SDataType();

		pDataType->eDataType         = EDataType::DTGeometry;
		pDataType->nLengthIndex      = EColumnMetaDataIndex::CMDIgnore;
		pDataType->fParseValidValues = false;

		this->m_mapDataTypeWrappers["geometry"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTGeometry
			);
		this->m_mapDataTypeWrappers["geometrycollection"] = std::make_shared<SDataTypeWrapper>(
				pDataType,
				EDataType::DTGeometryCollection
			);
	}
	#pragma endregion
}