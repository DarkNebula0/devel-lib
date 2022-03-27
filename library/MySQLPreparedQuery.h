#pragma once
#include <string>

#include "MySQLDatabase.h"

namespace MySQL {
	class CPreparedQuery {
	public:
		CPreparedQuery(std::shared_ptr<MySQL::CDatabase> i_pDatabase);
		~CPreparedQuery() { this->clear(); }

	public:
		MySQL::EError prepare(const std::string &i_sQuery) {
			this->m_sQuery = i_sQuery;
			return this->prepare();
		}
		MySQL::EError prepare();

		// Bind Params and Results after prepare and before execute! (When needed)
		MySQL::EError exec();
		MySQL::EError exec(MySQL::SPreparedBind *i_pResults, MySQL::SPreparedBind *i_pParams)
		{
			this->bind(i_pResults, i_pParams);
			return this->exec();
		}

		MySQL::SField *nextColumnMetaData() const;

		bool next();
		
		void clear(bool i_fExcludeQuery = false);

	public:
		void bind(MySQL::SPreparedBind *i_pResults, MySQL::SPreparedBind *i_pParams)
		{
			this->bindResults(i_pResults);
			this->bindParams(i_pParams);
		}
		void bindResults(MySQL::SPreparedBind *i_pResults) {
			this->m_pBinds[EPreparedBind::PBResult] = i_pResults;
		}
		void bindParams(MySQL::SPreparedBind *i_pParams) {
			this->m_pBinds[EPreparedBind::PBParam] = i_pParams;
		}

	public:
		bool isPrepared() const
		{
			return (this->m_pStatement != nullptr);
		}
		const std::string &query() const
		{
			return this->m_sQuery;
		}
		MySQL::EError nextError() const
		{
			return this->m_eNextError;
		}

		uint64 lastInsertID() const;
		std::string lastError() const;
		uint64 rows() const;
		uint fields() const;
		const MySQL::SPreparedBind *bind(const EPreparedBind i_eBind) const
		{
			return this->m_pBinds[i_eBind];
		}

	private:
		std::shared_ptr<MySQL::CDatabase>	 m_pDatabase;
		std::string							 m_sQuery;
		MySQL::SPreparedStatement			*m_pStatement;
		MySQL::EError						 m_eNextError;
		MySQL::SResult						*m_pMetaData;

		MySQL::SPreparedBind				*m_pBinds[EPreparedBind::PBMax];
	};
}