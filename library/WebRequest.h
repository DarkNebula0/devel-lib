#pragma once
#include "Typedef.h"

#include "map"
#include "string"

namespace Network
{
	namespace Web
	{
		class CWebRequest
		{
		friend class CWebSocket;
		public:
			CWebRequest();
			explicit CWebRequest(const CWebRequest *i_pOther) { this->operator=(i_pOther); }
			CWebRequest(const CWebRequest &i_oOther) { this->operator=(i_oOther); }
			CWebRequest(CWebRequest && i_oOther) noexcept { this->operator=(i_oOther); }
			virtual ~CWebRequest() = default;
		private:
			std::map<std::string, std::string> m_astHeader;
			std::string m_stContent;
		private:
			size_t m_nContentSize;
		public:
			void clear();
		public:
			CWebRequest& operator=(const CWebRequest *i_pOther);
			CWebRequest& operator=(CWebRequest && i_oOther) noexcept;
		public:
			_inline size_t contentSize() const { return this->m_nContentSize; }
			_inline const std::string &content() const { return this->m_stContent; }
			_inline bool hasContent() const { return !this->m_stContent.empty(); }
			_inline std::string getHeader(const std::string &i_stField) { return this->m_astHeader[i_stField]; }
		public:
			_inline void setHeader(const std::string &i_stField, const std::string &i_stArgument)
			{
				this->m_astHeader[i_stField] = i_stArgument;	
			}
			_inline void setContent(const std::string &i_stContent)
			{
				this->m_stContent = i_stContent;
				this->m_nContentSize = i_stContent.size();
			}
		public:
			_inline CWebRequest& operator=(const CWebRequest &i_oOther)
			{
				return this->operator=(&i_oOther);
			}
		};
	}
}