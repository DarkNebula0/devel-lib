#pragma once
#include <string>

#include "MySQLDatabase.h"

namespace MySQL {
	class CQuery {
	public:
		CQuery(const std::shared_ptr<MySQL::CDatabase> &i_pDatabase, const std::string &i_sQuery)
			: CQuery(i_pDatabase)
		{
			this->m_sQuery = i_sQuery;
		}
		CQuery(const std::shared_ptr<MySQL::CDatabase> &i_pDatabase)
			: m_pResult(nullptr)
		{
			this->clear();
			this->m_pDatabase = i_pDatabase;
		}
		~CQuery() { this->clear(); }

	public:
		MySQL::EError exec(const std::string &i_sQuery) {
			this->m_sQuery = i_sQuery;
			return this->exec();
		}
		MySQL::EError exec();

		bool next();
		void clear(bool i_fExcludeQuery = false);

		// Returns nullptr when not successfully
		static std::unique_ptr<MySQL::CQuery> fast(std::shared_ptr<MySQL::CDatabase> i_pDatabase, const std::string &i_sQuery);

	public:
		char *toRaw(const uint64 i_nIndex) const
		{
			return (this->isColumnValid(i_nIndex) ? this->m_pRow[i_nIndex] : nullptr);
		}
		template<typename T>
		T to(const uint64 i_nIndex) {
			if (this->isColumnValid(i_nIndex)) {
				return static_cast<T>((sizeof(T) == 4) ? Utils::StringToUInt(this->m_pRow[i_nIndex]) : Utils::StringToUInt64(this->m_pRow[i_nIndex]));
			}

			return static_cast<T>(0);
		}
		std::string toString(const uint64 i_nIndex) const
		{
			return (this->isColumnValid(i_nIndex) ? this->m_pRow[i_nIndex] : "");
		}
		double toDouble(const uint64 i_nIndex) const
		{
			return (this->isColumnValid(i_nIndex) ? atof(this->m_pRow[i_nIndex]) : 0.0);
		}
		float toFloat(const uint64 i_nIndex) const
		{
			return static_cast<float>(this->toDouble(i_nIndex));
		}
		
		MySQL::SField *nextColumnMetaData() const;

	public:
		const std::string &query() const
		{
			return this->m_sQuery;
		}
		uint64 lastInsertId() const
		{
			return this->m_nLastInsertID;
		}

		std::string lastError() const;
		uint64 rows() const;
		uint columns() const;

	private:
		bool isColumnValid(const uint64 i_nIndex) const
		{
			return (this->m_pResult && this->m_pRow && this->columns() > i_nIndex && this->m_pRow[i_nIndex]);
		}

	private:
		std::shared_ptr<MySQL::CDatabase>	 m_pDatabase;
		std::string							 m_sQuery;
		uint64								 m_nLastInsertID;
		MySQL::SResult						*m_pResult;
		MySQL::SRow							*m_pRow;
	};
}