#include "JsonObject.h"
#include "JsonArray.h"

Json::CObject::CObject()
	: m_eType(EJsonType::JTObject), m_pArray(nullptr)
{}

Json::CObject::~CObject()
{
	this->clear();
}

// Add object==============================================
void Json::CObject::addObject( const std::string &i_stName, CObject &&i_oObject )
{
	if (!i_stName.empty())
	{
		this->m_aoData[i_stName] = std::move(i_oObject);
	}
}

Json::CObject *Json::CObject::find(const std::string_view i_stName)
{
	InClassLock();
	std::string str(i_stName);
	return this->m_aoData.count(str) ? &this->m_aoData[str] : nullptr;
}

// Clear===================================================
void Json::CObject::clear()
{
	delete[] this->m_pArray;
	this->m_pArray = nullptr;
	this->m_eType = EJsonType::JTObject;
	this->m_aoData.clear();
	std::string::clear();
}

// Get keys================================================
std::vector<std::string> Json::CObject::getKeys()
{
	std::vector<std::string> astRet;
	
	InClassLock();
	astRet.reserve(this->m_aoData.size());
	for (auto &it : this->m_aoData)
	{
		astRet.emplace_back(it.first);
	}

	return astRet;
}

// To array================================================
Json::CArray Json::CObject::toArray() const
{
	if(this->m_pArray)
	{
		return *this->m_pArray;
	}
	return CArray();
}

Json::CObject &Json::CObject::operator=( const CObject &i_oOther )
{
	this->clear();
	
	this->m_eType = i_oOther.m_eType;
	this->m_aoData = i_oOther.m_aoData;
	

	if (i_oOther.m_pArray)
		this->m_pArray = new CArray(*i_oOther.m_pArray);

	std::string::operator=(i_oOther);
	return *this;
}

Json::CObject & Json::CObject::operator=( CObject &&i_oOther ) noexcept
{
	this->clear();
	this->m_eType = i_oOther.m_eType;
	this->m_aoData = i_oOther.m_aoData;
	this->m_pArray = i_oOther.m_pArray;
	std::string::operator=(i_oOther);

	i_oOther.m_pArray = nullptr;
	i_oOther.clear();
	return *this;
}

Json::CObject &Json::CObject::operator=(const CArray &i_oOther )
{
	this->m_eType = EJsonType::JTArray;

	if (this->m_pArray)
	{
		this->m_pArray->clear();		
	}
	else
	{
		this->m_pArray = new CArray();
	}

	this->m_pArray->operator=(i_oOther);
	return *this;
}