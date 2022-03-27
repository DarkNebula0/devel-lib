#pragma once

#include <string>

class Base64
{
public:
	Base64();
	Base64(char *i_pBuffer, const size_t i_nSize, bool i_bDeleteBuffer = false);
	explicit Base64(const std::string &i_stData);
	Base64(const Base64 &i_oOther) : Base64() { this->operator=(i_oOther); }
	explicit Base64(const Base64 *i_pOther) : Base64() { this->operator=(i_pOther); }
	Base64(Base64 &&i_oOther) noexcept : Base64() { this->operator=(i_oOther); }
	
	virtual ~Base64();
private:
	char* m_pBuffer;
private:
	size_t m_nSize;
private:
	bool m_bIsDeleteBuffer;
public:
	Base64& operator=(const Base64 *i_pOther);
	Base64& operator=(Base64 &&i_oOther) noexcept;
public:
	std::string encode() const;
	std::string decode() const;
public:
	_inline char *getBuffer() const { return this->m_pBuffer; }
	_inline size_t size() const { return this->m_nSize; }
	_inline bool isDeleteBuffer() const { return this->m_bIsDeleteBuffer; }
public:
	_inline void setDeleteBuffer(const bool i_bState) { this->m_bIsDeleteBuffer = i_bState; }
public:
	_inline Base64& operator=(const Base64 &i_oOther)
	{ return this->operator=(&i_oOther);}
};