#pragma once

#include "Typedef.h"
#include "Global.h"
#include <string>

namespace Network
{
	namespace Web
	{
		class CUrl
		{
		public:
			CUrl() = default;
			CUrl(const std::string &i_stUrl) { this->setUrl(i_stUrl); }
			CUrl(const CUrl &i_oOther){ this->operator=(i_oOther); }
			CUrl(const CUrl *i_pOther){ this->operator=(i_pOther);}
			virtual ~CUrl() = default;
		private:
			std::string m_stUrl;
			std::string m_stHost;
			std::string m_stResource;
			std::string m_stQuery;
			std::string m_stAuthorization;
		private:
			EProtocol m_eProtocol;
		private:
			ushort m_shPort;
		private:
			bool m_bIsValid;
			bool m_bIsSecure;
			bool m_bHasValidHostData;
		private:
			char m_achHostData[16];
		private:
			void clear();
			void extract();
		public:
			CUrl& operator =( const CUrl *i_pOther );
			CUrl& operator =(CUrl &&i_oOther);
			const sockaddr_in &getHostData();
		public:
			_inline const std::string	&url()				const { return this->m_stUrl; }
			_inline const std::string	&host()				const { return this->m_stHost; }
			_inline const std::string	&resource()			const { return this->m_stResource; }
			_inline const std::string	&query()			const { return this->m_stQuery; }
			_inline const std::string	&authorization()	const { return this->m_stAuthorization; }
			_inline EProtocol			protocol()			const { return this->m_eProtocol; }
			_inline ushort				port()				const { return this->m_shPort; }
			_inline bool				isValid()			const { return this->m_bIsValid; }
			_inline bool				isSecure()			const { return this->m_bIsSecure; }
			_inline bool				hasQuery()			const { return !this->m_stQuery.empty(); }
		public:
			_inline bool setUrl(const std::string &i_stUrl)
			{
				this->clear();
				this->m_stUrl = i_stUrl;
				this->extract();
				return this->m_bIsValid;
			}
			_inline CUrl& operator =(const CUrl &i_oIther)
			{
				return this->operator=(&i_oIther);
			}
		};
	}
}