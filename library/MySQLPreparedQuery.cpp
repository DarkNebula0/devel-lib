#include "MySQLPreparedQuery.h"

#include "../external/mysql/include/mysql.h"


MySQL::CPreparedQuery::CPreparedQuery(std::shared_ptr<MySQL::CDatabase> i_pDatabase)
	: m_pStatement(nullptr)
{
	this->clear();
	this->m_pDatabase = i_pDatabase;
}


MySQL::EError MySQL::CPreparedQuery::prepare()
{
	this->clear(true);

	LockMySQLDB(this->m_pDatabase);

	if ((this->m_pStatement = mysql_stmt_init(this->m_pDatabase->raw())) == nullptr) {
		return MySQL::EError::ELastError;
	}
	else if (mysql_stmt_prepare(this->m_pStatement, this->m_sQuery.c_str(), static_cast<uint>(this->m_sQuery.length())) != 0) {
		return MySQL::EError::ELastError;
	}

	this->m_pMetaData = mysql_stmt_result_metadata(this->m_pStatement);

	return MySQL::EError::ESuccess;
}

MySQL::EError MySQL::CPreparedQuery::exec()
{
	if (!this->m_pStatement) {
		return MySQL::EError::ENotPrepared;
	}

	LockMySQLDB(this->m_pDatabase);

	if (this->m_pBinds[0]) {
		mysql_stmt_bind_param(this->m_pStatement, this->m_pBinds[0]);
	}
	if (this->m_pBinds[1]) {
		mysql_stmt_bind_result(this->m_pStatement, this->m_pBinds[1]);
	}

	if (mysql_stmt_execute(this->m_pStatement) != 0) {
		return MySQL::EError::ELastError;
	}
	else if (this->m_pBinds[1] && mysql_stmt_store_result(this->m_pStatement) != 0) {
		return MySQL::EError::ELastError;
	}

	return MySQL::EError::ESuccess;
}

MySQL::SField *MySQL::CPreparedQuery::nextColumnMetaData() const
{
	return (this->m_pMetaData ? mysql_fetch_field(this->m_pMetaData) : nullptr);
}

bool MySQL::CPreparedQuery::next()
{
	if (this->m_pStatement && this->m_pBinds[1]) {
		int iError = mysql_stmt_fetch(this->m_pStatement);
		if (iError == 0) {
			this->m_eNextError = MySQL::EError::ESuccess;
			return true;
		}
		else if (iError == 1) {
			
			this->m_eNextError = MySQL::EError::ELastError;
		}
		else if (iError == MySQL::EError::ENoData)
		{
			this->m_eNextError = MySQL::EError::ENoData;
		}
		else {
			this->m_eNextError = static_cast<MySQL::EError>(iError);
		}
	}
	else {
		this->m_eNextError = MySQL::EError::ENoResultBound;
	}

	return false;
}


void MySQL::CPreparedQuery::clear(const bool i_fExcludeQuery)
{
	if (this->m_pStatement) {
		mysql_stmt_free_result(this->m_pStatement);
		mysql_free_result(this->m_pMetaData);
		mysql_stmt_close(this->m_pStatement);
	}

	this->m_eNextError = MySQL::EError::ENothing;
	this->m_pStatement = nullptr;
	this->m_pBinds[0] = this->m_pBinds[1] = nullptr;
	this->m_pMetaData = nullptr;

	if (!i_fExcludeQuery) {
		this->m_sQuery.clear();
	}
}


uint64 MySQL::CPreparedQuery::lastInsertID() const
{
	return (this->m_pStatement ? mysql_stmt_insert_id(this->m_pStatement) : (~0));
}

std::string MySQL::CPreparedQuery::lastError() const
{
	return this->m_pDatabase->lastError();
}

uint64 MySQL::CPreparedQuery::rows() const
{
	return (this->m_pStatement ? mysql_stmt_num_rows(this->m_pStatement) : 0);
}

uint MySQL::CPreparedQuery::fields() const
{
	return (this->m_pStatement ? mysql_stmt_field_count(this->m_pStatement) : 0);
}
