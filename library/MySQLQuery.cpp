#include "MySQLQuery.h"
#include "Global.h"

#include "../external/mysql/include/mysql.h"


MySQL::EError MySQL::CQuery::exec()
{
	this->clear(true);

	// Lock the database for no errors and half deadlock-safety
	LockMySQLDB(this->m_pDatabase);
	
	// Execute the query in Database
	if (mysql_real_query(this->m_pDatabase->raw(), this->m_sQuery.c_str(), static_cast<uint>(this->m_sQuery.length())) != 0) {
		return MySQL::EError::ELastError;
	}

	// Get the results of the executed query
	this->m_pResult = mysql_store_result(this->m_pDatabase->raw());

	// Try to retrieve the insert ID
	if ((this->m_nLastInsertID = mysql_insert_id(this->m_pDatabase->raw()) == 0)) {
		this->m_nLastInsertID = (~0);
	}
	
	return MySQL::EError::ESuccess;
}

bool MySQL::CQuery::next()
{
	// Check if a query is executed
	if (!this->m_pResult) {
		return false;
	}
	// Get the next result row
	else if (!(this->m_pRow = mysql_fetch_row(this->m_pResult))) {
		return false;
	}

	return true;
}


void MySQL::CQuery::clear(bool i_fExcludeQuery)
{
	if (this->m_pResult) {
		mysql_free_result(this->m_pResult);
	}
	
	this->m_nLastInsertID = (~0);
	this->m_pResult = nullptr;
	this->m_pRow = nullptr;

	if (!i_fExcludeQuery) {
		this->m_sQuery.clear();
	}
}

std::unique_ptr<MySQL::CQuery> MySQL::CQuery::fast(std::shared_ptr<MySQL::CDatabase> i_pDatabase, const std::string &i_sQuery)
{
	std::unique_ptr<MySQL::CQuery> pQuery = std::make_unique<MySQL::CQuery>(i_pDatabase, i_sQuery);
	if (pQuery->exec() != MySQL::EError::ESuccess) {
		pQuery = nullptr;
	}

	return pQuery;
}


MySQL::SField *MySQL::CQuery::nextColumnMetaData() const
{
	return (this->m_pResult ? mysql_fetch_field(this->m_pResult) : nullptr);
}

std::string MySQL::CQuery::lastError() const
{
	return this->m_pDatabase->lastError();
}

uint64 MySQL::CQuery::rows() const
{
	return (this->m_pResult ? mysql_num_rows(this->m_pResult) : 0);
}

uint MySQL::CQuery::columns() const
{
	return (this->m_pResult ? mysql_num_fields(this->m_pResult) : 0);
}
