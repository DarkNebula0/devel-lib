#pragma once
#include "Global.h"

template<size_t Size>
class CCharArray
{
public:
	CCharArray()
		: m_szString{ 0 }
	{
	}
	CCharArray(const char *i_szString)
	{
		Utils::StringToArray(this->m_szString, i_szString);
	}
	CCharArray(const std::string &i_sString)
	{
		Utils::StringToArray(this->m_szString, i_sString);
	}
	CCharArray(std::initializer_list<char> i_oList)
	{
		size_t nSize = i_oList.size();

		if (nSize > Size)
		{
			nSize = Size;
		}

		Utils::StringToArray(this->m_szString, i_oList.begin(), nSize);
	}
	CCharArray(const CCharArray<Size> &i_szString)
	{
		this->operator=(i_szString);
	}
	template<size_t TOtherSize>
	CCharArray(const CCharArray<TOtherSize> &i_szString)
	{
		this->operator=(i_szString);
	}
	~CCharArray() {}

public:
	void set(const std::string &i_sString)
	{
		Utils::StringToArray(this->m_szString, i_sString);
	}

	void set(const char *i_szString, size_t i_nSize)
	{
		Utils::StringToArray(this->m_szString, i_szString, i_nSize);
	}

	void set(const char *i_szString)
	{
		Utils::StringToArray(this->m_szString, i_szString);
	}

	void set(const CCharArray<Size> &i_szString)
	{
		return this->set(i_szString.c_str(), i_szString.length());
	}

	template<size_t SizeOther>
	void set(const CCharArray<SizeOther> &i_szString)
	{
		return this->set(i_szString.c_str(), i_szString.length());
	}

public:
	char *begin()
	{
		return &this->first();
	}
	const char *begin() const
	{
		return &this->first();
	}

	char *end()
	{
		return &this->first() + Size;
	}
	const char *end() const
	{
		return &this->first() + Size;
	}

	const char *c_str() const
	{
		return this->m_szString;
	}

public:
	size_t length() const
	{
		return Utils::StringSize(this->m_szString, Size);
	}

	static constexpr size_t maxLength()
	{
		return Size;
	}

public:
	char &first()
	{
		return this->m_szString[0];
	}
	const char &first() const
	{
		return this->m_szString[0];
	}

	char &last()
	{
		return this->m_szString[Size - 1];
	}
	const char &last() const
	{
		return this->m_szString[Size - 1];
	}

	char &at(const size_t i_nIndex)
	{
		return this->m_szString[i_nIndex];
	}
	const char &at(const size_t i_nIndex) const
	{
		return this->m_szString[i_nIndex];
	}

public:
	std::string toStdString() const
	{
		return std::string(this->m_szString, this->length());
	}

public:
	CCharArray<Size> &operator=(const CCharArray<Size> &i_szString)
	{
		memcpy(this->m_szString, i_szString.c_str(), Size);
		return *this;
	}

	template<size_t TOherSize>
	CCharArray<Size> &operator=(const CCharArray<TOherSize> &i_szString)
	{
		this->set<TOherSize>(i_szString);
		return *this;
	}

	CCharArray<Size> &operator=(const std::string &i_sString)
	{
		this->set(i_sString);
		return *this;
	}

	CCharArray<Size> &operator=(const char *i_szString)
	{
		this->set(i_szString);
		return *this;
	}

	bool operator==(const CCharArray<Size> &i_szString) const
	{
		return this->compare(i_szString.c_str(), i_szString.length());
	}
	template<size_t SizeOther>
	bool operator==(const CCharArray<SizeOther> &i_szString) const
	{
		return this->compare(i_szString.c_str(), i_szString.length());
	}
	bool operator==(const char *i_szString) const
	{
		return this->compare(i_szString, strlen(i_szString));
	}
	bool operator==(const std::string &i_sString) const
	{
		return this->compare(i_sString.c_str(), i_sString.length());
	}

	/*friend bool operator==(const std::string &i_sString, const CCharArray<Size> &i_szString)
	{
		return (i_szString == i_sString);
	}
	friend bool operator==(const char *i_pString, const CCharArray<Size> &i_szString)
	{
		return (i_szString == i_pString);
	}*/


	bool operator!=(const CCharArray<Size> &i_szString) const
	{
		return !this->compare(i_szString.c_str(), i_szString.length());
	}
	template<size_t SizeOther>
	bool operator!=(const CCharArray<SizeOther> &i_szString) const
	{
		return !this->compare(i_szString.c_str(), i_szString.length());
	}
	bool operator!=(const char *i_szString) const
	{
		return !this->compare(i_szString, strlen(i_szString));
	}
	bool operator!=(const std::string &i_sString) const
	{
		return !this->compare(i_sString.c_str(), i_sString.length());
	}

	/*friend bool operator!=(const std::string &i_sString, const CCharArray<Size> &i_szString)
	{
		return (i_szString != i_sString);
	}
	friend bool operator!=(const char *i_pString, const CCharArray<Size> &i_szString)
	{
		return (i_szString != i_pString);
	}*/


	operator char *()
	{
		return this->m_szString;
	}
	operator const char *() const
	{
		return this->m_szString;
	}

private:
	bool compare(const char *i_szOther, const size_t i_nLength) const
	{
		const size_t nLength = this->length();
		if (i_nLength != nLength) return false;

		return (memcmp(this->m_szString, i_szOther, nLength) == 0);
	}

private:
	char m_szString[Size];
};