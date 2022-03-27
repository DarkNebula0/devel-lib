#include "MySQLDatabase.h"
#include "MySQLQuery.h"
#include "../external/mysql/include/mysql.h"

#pragma comment(lib, "../external/mysql/lib/libmysql.lib")

MySQL::EError MySQL::CDatabase::connect()
{
	if (this->isOpen()) {
		return MySQL::EError::EAlreadyOpen;
	}

	if (!this->m_pDatabase) {
		this->m_pDatabase = std::shared_ptr<MySQL::SDatabase>(mysql_init(0), [](MySQL::SDatabase *i_pDatabase) {
			mysql_close(i_pDatabase);
		});

		if (!this->m_pDatabase) {
			return MySQL::EError::ENotInitializable;
		}
	}

	mysql_options(this->raw(), MYSQL_OPT_RECONNECT, &this->m_oData.fAutoReconnect);

	bool isOpen(mysql_real_connect(this->raw(), this->m_oData.sHost.c_str(), this->m_oData.sUsername.c_str(), this->m_oData.sPassword.c_str(), this->m_oData.sDatabase.c_str(), this->m_oData.wPort, 0, 0) == this->raw());
	if (!isOpen) {
		return MySQL::EError::ELastError;
	}

	return MySQL::EError::ESuccess;
}

void MySQL::CDatabase::close()
{
	if (this->m_pDatabase) {
		this->m_pDatabase = nullptr;
	}
}


bool MySQL::CDatabase::startTransaction() {
	if (!this->isOpen()) return false;

	if (!this->m_fInTransaction) {
		MySQL::CQuery oQuery(this->shared_from_this());
		oQuery.exec("SET autocommit=0");
		oQuery.exec("START TRANSACTION");

		this->m_fInTransaction = true;
	}

	return true;
}

bool MySQL::CDatabase::commit(bool i_fStopTransaction) {
	if (!this->isOpen() || !this->m_fInTransaction) return false;

	MySQL::CQuery oQuery(this->shared_from_this());
	oQuery.exec("COMMIT");
	
	if (i_fStopTransaction) stopTransaction();

	return true;
}

bool MySQL::CDatabase::rollback(bool i_fStopTransaction) {
	if (!this->isOpen() || !this->m_fInTransaction) return false;

	MySQL::CQuery oQuery(this->shared_from_this());
	oQuery.exec("ROLLBACK");

	if (i_fStopTransaction) {
		stopTransaction();
	}

	return true;
}

bool MySQL::CDatabase::stopTransaction() {
	if (!this->isOpen() || !this->m_fInTransaction) return false;

	MySQL::CQuery oQuery(this->shared_from_this());
	oQuery.exec("SET autocommit=1");

	this->m_fInTransaction = false;

	return true;
}


bool MySQL::CDatabase::isOpen()
{
	return (this->m_pDatabase ? (mysql_ping(this->raw()) == 0) : false);
}

std::string MySQL::CDatabase::lastError()
{
	if (this->m_pDatabase) {
		const char *pLastError = mysql_error(this->raw());
		if (pLastError) {
			return pLastError;
		}
	}

	return "";
}