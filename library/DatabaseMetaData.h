#pragma once
#include "MutexVector.h"
#include <string>
#include <memory>

#include "DatabaseColumnMetaData.h"


namespace MySQL
{
	namespace ORM
	{
		class CTableMetaData
		{
		public:
			CTableMetaData(const std::string &i_sName)
				: m_sName(i_sName)
			{}

		public:
			const std::string &name() const
			{
				return this->m_sName;
			}

		public:
			void addColumn(const std::shared_ptr<SColumnMetaData> &i_pColumnMetaData)
			{
				this->m_apColumnMetaData.push_back(i_pColumnMetaData);
			}

			std::shared_ptr<SColumnMetaData> columnMetaData(std::string i_sName)
			{
				Utils::StringToLower(i_sName);

				MutexVectorLockGuard(this->m_apColumnMetaData);
				for (const std::shared_ptr<SColumnMetaData> &pData : this->m_apColumnMetaData)
				{
					if (pData->sName == i_sName)
					{
						return pData;
					}
				}

				return nullptr;
			}

			std::shared_ptr<SColumnMetaData> columnMetaData(const size_t i_nPosition)
			{
				MutexVectorLockGuard(this->m_apColumnMetaData);
				for (const std::shared_ptr<SColumnMetaData> &pData : this->m_apColumnMetaData)
				{
					if (pData->nPosition == i_nPosition)
					{
						return pData;
					}
				}

				return nullptr;
			}

			size_t columnCount() const
			{
				return this->m_apColumnMetaData.size();
			}

		private:
			std::string m_sName;
			CMutexVector<std::shared_ptr<SColumnMetaData>> m_apColumnMetaData;
		};

		class CMetaData
		{
		public:
			CMetaData(const std::string &i_sName = "")
				: m_sName(i_sName)
			{}

		public:
			void setName(const std::string &i_sName)
			{
				this->m_sName = i_sName;
			}

			const std::string &name() const
			{
				return this->m_sName;
			}

		public:
			void addTable(const std::shared_ptr<CTableMetaData> &i_pTableMetaData)
			{
				this->m_apTableMetaData.push_back(i_pTableMetaData);
			}

			std::shared_ptr<CTableMetaData> tableMetaData(std::string i_sName)
			{
				Utils::StringToLower(i_sName);

				MutexVectorLockGuard(this->m_apTableMetaData);
				for (const std::shared_ptr<CTableMetaData> &pData : this->m_apTableMetaData)
				{
					if (pData->name() == i_sName)
					{
						return pData;
					}
				}

				return nullptr;
			}

			CMutexVector<std::shared_ptr<CTableMetaData>> &tables()
			{
				return this->m_apTableMetaData;
			}

		private:
			std::string m_sName;
			CMutexVector<std::shared_ptr<CTableMetaData>> m_apTableMetaData;
		};
	}
}