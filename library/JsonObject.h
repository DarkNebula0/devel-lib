#pragma once

#include "GlobalStringUtils.h"
#include "LockGuard.h"
#include "Typedef.h"
#include "Global.h"
#include "Mutex.h"

#include <string_view>
#include <string>
#include <map>

namespace Json
{
	class CObject : public std::string
	{
	#define InClassLock() RecursiveLockGuard(this->m_oMutex)
		friend class CDocument;
	public:
		CObject();
		CObject( const CObject &i_oOther ) : CObject()
		{
			this->operator=(i_oOther);
		}
		CObject (CObject &&i_oOther) : CObject()
		{
			this->operator=(std::move((i_oOther)));
		}
		virtual ~CObject();
	private:
		EJsonType m_eType;
	private:
		CMutex m_oMutex;
		CArray *m_pArray;
	private:
		std::map<std::string, CObject > m_aoData;
	private:
		void addObject (const std::string &i_stName, CObject &&i_oObject);
		void setType(const EJsonType i_eType) { this->m_eType = i_eType; }
		void clear();
	public:
		CObject *find(const std::string_view i_stName);
		std::vector<std::string> getKeys();
		CArray toArray() const;
	public:
		_inline CMutex		&mutex()
		{
			return this->m_oMutex;
		}
		_inline EJsonType	type() const { return this->m_eType; }
		_inline CObject		&get(const std::string_view i_stName) { return this->operator[](i_stName); }
	public:
		_inline void setNull() { this->m_eType = EJsonType::JTNull; }
	public:
		_inline uint64	toUInt64()	const { return Utils::StringToUInt64(*this); }
		_inline int64	toInt64()		const { return Utils::StringToInt64(*this); }
		_inline int		toInt()			const { return Utils::StringToInt(*this); }
		_inline uint	toUInt()		const { return Utils::StringToInt(*this); }
		_inline float	toFloat()		const { return Utils::StringToFloat(*this); }
		_inline double	toDouble()	const { return Utils::StringToDouble(*this); }
	public:
		 CObject &operator=( const CObject &i_oOther );
		 CObject &operator=(CObject &&i_oOther) noexcept;
		 CObject &operator=(const CArray &i_oOther);
	public:
		_inline CObject &operator=(const char* i_stValue)
		{
			return this->operator=(std::string(i_stValue));
		}
		_inline CObject &operator=(const std::string &i_stValue)
		{
			this->m_eType = EJsonType::JTString;
			std::string::operator=(i_stValue);
			return *this;
		}
		_inline CObject &operator=(const uint64 i_nValue)
		{
			this->m_eType = EJsonType::JTNumber;
			std::string::operator=(std::to_string(i_nValue));
			return *this;
		}
		_inline CObject &operator=(const uint i_nValue)
		{
			this->m_eType = EJsonType::JTNumber;
			std::string::operator=(std::to_string(i_nValue));
			return *this;
		}		
		_inline CObject &operator=(const int64 i_nValue)
		{
			this->m_eType = EJsonType::JTNumber;
			std::string::operator=(std::to_string(i_nValue));
			return *this;
		}
		_inline CObject &operator=(const float i_fValue)
		{
			this->m_eType = EJsonType::JTNumber;
			std::string::operator=(std::to_string(i_fValue));
			return *this;
		}
		_inline CObject &operator=(const double i_dValue)
		{
			this->m_eType = EJsonType::JTNumber;
			std::string::operator=(std::to_string(i_dValue));
			return *this;
		}
		_inline CObject &operator=(const int i_nValue)
		{
			this->m_eType = EJsonType::JTNumber;
			std::string::operator=(std::to_string(i_nValue));
			return *this;
		}
		_inline CObject &operator=(const bool i_bState)
		{
			 this->operator=((i_bState ? 1 : 0));
			 this->m_eType = EJsonType::JTBoolean;
			 return *this;
		}
		_inline CObject &operator[](const std::string_view i_stKey)
		{
			InClassLock();
			return this->m_aoData.operator[](std::string(i_stKey));
		}
	};
}
