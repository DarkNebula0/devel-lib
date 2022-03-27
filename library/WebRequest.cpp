#include "WebRequest.h"

Network::Web::CWebRequest::CWebRequest() 
	: m_nContentSize(0)
{}

void Network::Web::CWebRequest::clear()
{
	this->m_astHeader.clear();
	this->m_stContent.clear();
	this->m_nContentSize = 0;
}

Network::Web::CWebRequest & Network::Web::CWebRequest::operator=(const CWebRequest *i_pOther)
{
	if (i_pOther)
	{
		this->m_astHeader = i_pOther->m_astHeader;
		this->m_stContent = i_pOther->m_stContent;
		this->m_nContentSize = i_pOther->m_nContentSize;
	}
	return *this;
}

Network::Web::CWebRequest & Network::Web::CWebRequest::operator=(CWebRequest &&i_oOther) noexcept
{
	this->m_astHeader = std::move(i_oOther.m_astHeader);
	this->m_stContent = std::move(i_oOther.m_stContent);
	this->m_nContentSize = i_oOther.m_nContentSize;
	
	i_oOther.clear();
	return *this;
}
