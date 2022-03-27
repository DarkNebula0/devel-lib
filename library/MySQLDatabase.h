#pragma once
#include <string>
#include <memory>

#include "LockGuard.h"
#include "Typedef.h"
#include "Global.h"

namespace MySQL {
#define LockMySQLDB(x)	RecursiveLockGuard(x->mutex())

	class CDatabase : public std::enable_shared_from_this<CDatabase> {
		friend class MySQL::CPreparedQuery;
		friend class MySQL::CQuery;
	public:
		CDatabase()
			: m_fInTransaction(false)
		{ }
		CDatabase(const std::string &i_sHost, const std::string &i_sUsername, const std::string &i_sPassword, const std::string &i_sDatabase, ushort i_wPort = 3306, bool i_fAutoReconnect = true)
			: CDatabase()	{ this->setConnectionData(i_sHost, i_sUsername, i_sPassword, i_sDatabase, i_wPort, i_fAutoReconnect); }
		CDatabase(const MySQL::SConnectionData &i_oDatabaseData)
			: CDatabase()	{ this->m_oData = i_oDatabaseData; }

		CDatabase(const MySQL::CDatabase &i_oDatabase) = delete;
		CDatabase(const MySQL::CDatabase *i_pDatabase) = delete;
		
		~CDatabase() { this->close(); }

	public:
		template<class TDatabase = MySQL::CDatabase>
		static std::shared_ptr<TDatabase> createInstance(const std::string &i_sHost, const std::string &i_sUsername, const std::string &i_sPassword, const std::string &i_sDatabase, ushort i_wPort = 3306, bool i_fAutoReconnect = true)
		{
			std::shared_ptr<TDatabase> pInstance = MySQL::CDatabase::createInstance();

			pInstance->setConnectionData(i_sHost, i_sUsername, i_sPassword, i_sDatabase, i_wPort, i_fAutoReconnect);

			return pInstance;
		}

		template<class TDatabase = MySQL::CDatabase>
		static std::shared_ptr<TDatabase> createInstance(const MySQL::SConnectionData &i_oDatabaseData)
		{
			std::shared_ptr<TDatabase> pInstance = MySQL::CDatabase::createInstance();

			pInstance->setConnectionData(i_oDatabaseData);

			return pInstance;
		}

		template<class TDatabase = MySQL::CDatabase>
		static std::shared_ptr<TDatabase> createInstance()
		{ return std::make_shared<TDatabase>(); }

	public:
		MySQL::EError connect(const std::string &i_sHost, const std::string &i_sUsername, const std::string &i_sPassword, const std::string &i_sDatabase, ushort i_wPort = 3306, bool i_fAutoReconnect = true) {
			this->setConnectionData(i_sHost, i_sUsername, i_sPassword, i_sDatabase, i_wPort, i_fAutoReconnect);
			return this->connect();
		}
		MySQL::EError connect(const MySQL::SConnectionData &i_oDatabaseData) {
			this->setConnectionData(i_oDatabaseData);
			return this->connect();
		}
		MySQL::EError connect();

		void close();

	public:
		bool startTransaction();
		bool commit(bool i_fStopTransaction = true);
		bool rollback(bool i_fStopTransaction = true);
		bool stopTransaction();

	public:
		bool isOpen();
		std::string lastError();
		const SConnectionData &connectionData() const
		{
			return this->m_oData;
		}

	private:
		MySQL::SDatabase *raw() const
		{
			return this->m_pDatabase.get();
		}
		const CMutex &mutex() const
		{
			return this->m_oMutex;
		}

	private:
		void setConnectionData(const std::string &i_sHost, const std::string &i_sUsername, const std::string &i_sPassword, const std::string &i_sDatabase, ushort i_wPort, bool i_fAutoReconnect) {
			this->m_oData.sHost = i_sHost;
			this->m_oData.sUsername = i_sUsername;
			this->m_oData.sPassword = i_sPassword;
			this->m_oData.sDatabase = i_sDatabase;
			this->m_oData.wPort = i_wPort;
			this->m_oData.fAutoReconnect = i_fAutoReconnect;
		}
		void setConnectionData(const MySQL::SConnectionData &i_oData)
		{
			this->m_oData = i_oData;
		}

	private:
		std::shared_ptr<MySQL::SDatabase>	m_pDatabase;
		MySQL::SConnectionData				m_oData;
		CMutex								m_oMutex;
		bool								m_fInTransaction;
	};
}