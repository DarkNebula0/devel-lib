#pragma once
#include <memory>

#include "DatabaseORMHelperFunctions.h"
#include "DatabaseDataTypeWrapper.h"
#include "DatabaseORMQueryBuilder.h"
#include "DatabaseSerializer.h"
#include "DatabaseORMTable.h"
#include "DatabaseMetaData.h"
#include "MySQLDatabase.h"
#include "MutexVector.h"
#include "MySQLQuery.h"


namespace MySQL
{
	namespace ORM
	{
		class CORM
		{
		public:
			CORM(const std::shared_ptr<CMySQLDatabase> i_pDatabase = nullptr)
				: m_fIsInitialized(false)
			{
				this->setDatabase(i_pDatabase);
			}

		public:
			bool initialize()
			{
				if (!this->m_pDatabase)
					throw CNoDatabasePassedException();

				if (this->m_fIsInitialized)
					return true;

				if (this->m_pDatabase->isOpen() || this->m_pDatabase->connect() == MySQL::EError::ESuccess)
				{
					this->retrieveTables();
					this->retrieveMetaData();

					this->m_fIsInitialized = true;
					return true;
				}

				return false;
			}

		public:
			// TClass has to inherit CTable<YourModel>
			template <typename TClass, typename std::enable_if_t<std::is_base_of_v<CTable<typename TClass::RawModel>, TClass>> * = nullptr>
			std::shared_ptr<TClass> newTable(const std::string &i_sName)
			{
				if (std::shared_ptr<CTableMetaData> pTableMetaData = this->m_oMetaData.tableMetaData(i_sName))
				{
					std::shared_ptr<TClass> pClass = std::make_shared<TClass>();
					pClass->setTableData(this, pTableMetaData);

					std::shared_ptr<CTable<>> pBaseClass = std::reinterpret_pointer_cast<CTable<>>(pClass);
					this->m_apTables.push_back(std::move(pBaseClass));

					return pClass;
				}

				return nullptr;
			}

			template <typename TClass>
			std::shared_ptr<TClass> getTable(std::string i_sName)
			{
				Utils::StringToLower(i_sName);

				ForEachMV(const std::shared_ptr<CTable<>> &pTable, this->m_apTables)
				{
					if (pTable->metaData()->name() == i_sName)
					{
						return std::reinterpret_pointer_cast<TClass>(pTable);
					}
				}

				return nullptr;
			}

			bool removeTable(std::string i_sName)
			{
				Utils::StringToLower(i_sName);

				ForMVDefault(this->m_apTables)
				{
					std::shared_ptr<CTable<>> &pTable = this->m_apTables[i];

					if (pTable->metaData()->name() == i_sName)
					{
						this->m_apTables.removeAt(i);
						pTable->setTableData(nullptr, nullptr);
						return true;
					}
				}

				return false;
			}


		private:
			void retrieveTables()
			{
				MySQL::CQuery oQuery(m_pDatabase);
				if (oQuery.exec("SHOW TABLES") == MySQL::EError::ESuccess)
				{
					while (oQuery.next())
					{
						const std::shared_ptr<CTableMetaData> pTable = std::make_shared<CTableMetaData>(Utils::StringToLowerCopy(oQuery.toString(0)));
						this->m_oMetaData.addTable(pTable);
					}
				}
			}

			void retrieveMetaData()
			{
				CMySQLQuery oQuery(this->m_pDatabase);

				ForEachMV(const std::shared_ptr<CTableMetaData> &pTable, this->m_oMetaData.tables())
				{
					if (oQuery.exec("SELECT"
						" COLUMN_NAME,ORDINAL_POSITION,IS_NULLABLE,DATA_TYPE,CHARACTER_MAXIMUM_LENGTH,NUMERIC_PRECISION,NUMERIC_SCALE,COLUMN_TYPE,COLUMN_KEY,EXTRA"
						" FROM INFORMATION_SCHEMA.COLUMNS"
						" WHERE TABLE_SCHEMA='" + this->m_oMetaData.name() + "' AND TABLE_NAME='" + pTable->name() + "'") == MySQL::EError::ESuccess)
					{
						while (oQuery.next())
						{
							std::shared_ptr<SColumnMetaData> pMetaData = std::make_shared<SColumnMetaData>();

							pMetaData->sName = oQuery.toString(EColumnMetaDataIndex::CMDColumnName);
							pMetaData->nPosition = oQuery.to<ushort>(EColumnMetaDataIndex::CMDOrdinalPosition);
							pMetaData->fIsNullable = (oQuery.toString(EColumnMetaDataIndex::CMDIsNullable) == "YES");
							pMetaData->nLength = pMetaData->nPrecision = 0;

							const std::string sDataType = Utils::StringToLowerCopy(oQuery.toString(EColumnMetaDataIndex::CMDDataType));
							const std::shared_ptr<SDataTypeWrapper> pDataTypeWrapper = ColumnDataTypeWrapperInstance->dataTypeWrapper(sDataType);
							if (pDataTypeWrapper)
							{
								pMetaData->pDataTypeWrapper = pDataTypeWrapper;

								const EColumnMetaDataIndex eMetaDataIndex = pMetaData->pDataTypeWrapper->pDataType->nLengthIndex;
								if (eMetaDataIndex != EColumnMetaDataIndex::CMDIgnore)
								{
									pMetaData->nLength = oQuery.to<uint64>(eMetaDataIndex);
									if (eMetaDataIndex == EColumnMetaDataIndex::CMDNumericPrecision)
									{
										pMetaData->nPrecision = oQuery.to<uint64>(EColumnMetaDataIndex::CMDNumericScale);
									}
								}
							}
							else
							{
								throw CDataTypeNotFoundException();
							}

							const std::string sColumnType = oQuery.toString(EColumnMetaDataIndex::CMDColumnType);
							pMetaData->fIsUnsigned = Utils::SearchInStr(sColumnType, " unsigned");
							pMetaData->fIsPrimaryKey = (oQuery.toString(EColumnMetaDataIndex::CMDColumnKey) == "PRI");

							if (pMetaData->pDataTypeWrapper && pMetaData->pDataTypeWrapper->pDataType->fParseValidValues)
							{
								size_t nPositionValues[2] = { sColumnType.find_first_of('('), sColumnType.find_first_of(')') };
								if (nPositionValues[0] != std::string::npos && nPositionValues[1] != std::string::npos)
								{
									pMetaData->asValidValues = Utils::Split(sColumnType.substr(nPositionValues[0] + 1, nPositionValues[1] - nPositionValues[0] - 1), ',', 10);
									for (std::string &sValue : pMetaData->asValidValues)
									{
										sValue.erase(sValue.begin());
										sValue.erase(sValue.end() - 1);
									}
								}
							}

							const std::string sExtra = oQuery.toString(EColumnMetaDataIndex::CMDExtra);
							pMetaData->fIsAutoIncrement = Utils::SearchInStr(sExtra, "auto_increment");
							pMetaData->fIsVirtual = Utils::SearchInStr(sExtra, "VIRTUAL");

							pTable->addColumn(pMetaData);
						}
					}
				}
			}

		public:
			void setDatabase(const std::shared_ptr<CMySQLDatabase> &i_pDatabase)
			{
				if (this->m_fIsInitialized)
				{
					throw CORMIsAlreadyInitializedException();
				}

				if (i_pDatabase)
				{
					this->m_oMetaData.setName(Utils::StringToLowerCopy(i_pDatabase->connectionData().sDatabase));
				}

				this->m_pDatabase = i_pDatabase;
			}

			std::shared_ptr<CMySQLDatabase> database() const
			{
				return this->m_pDatabase;
			}

		private:
			bool m_fIsInitialized;
			std::shared_ptr<CMySQLDatabase> m_pDatabase;
			CMetaData m_oMetaData;

			CMutexVector<std::shared_ptr<CTable<>>> m_apTables;
		};
	}
}