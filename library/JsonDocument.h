#pragma once

#include "JsonObject.h"
#include "Typedef.h"
#include "Global.h"

#include <string>

namespace Json
{
	class CDocument
	{
	public:
		CDocument();
		CDocument(const std::string &i_stJson);
		CDocument(const CDocument &i_oOther) : CDocument() { this->operator=(i_oOther); }
		CDocument(CDocument &&i_oOther) noexcept : CDocument() { this->operator=(std::move(i_oOther)); }
		virtual ~CDocument();
	private:
		CObject m_oObject;
	private:
		char *m_pBuffer;
		char *m_pPointer;
		char *m_pEnd;
	private:
		void removeWhitespace();
		void selectParser(std::string &i_stOutBuffer, CObject *i_pObject = nullptr);
	private:
		uint parseUnicodeHex();
	private:
		static void parseNull(std::string &i_stOutBuffer, CObject *i_pObject);
		void parseBool(std::string &i_stOutBuffer, CObject *i_pObject);
		void parseObject(std::string &i_stOutBuffer, CObject *i_pObject);
		void parseString(std::string &i_stOutBuffer, CObject *i_pObject = nullptr);
		void parseNumber(std::string &i_stOutBuffer, CObject *i_pObject);
		void parseArray(CObject *i_pObject);
	private:
		static void selectSerializer(std::string &i_stBuffer, CObject &i_oObject, bool i_bFormatted = false);
		static void serializeArray(std::string &i_stBuffer, CObject &i_oObject, bool i_bFormatted = false);
		static void serializeObject(std::string &i_stBuffer, CObject &i_oObject, bool i_bFormatted = false);
		static void serializeNumber(std::string &i_stBuffer, CObject &i_oObject);
		static void serializeString(std::string &i_stBuffer, CObject &i_oObject);
		static void serializeNull(std::string &i_stBuffer);
		static void serializeBool(std::string &i_stBuffer, CObject &i_oObject);
	private:
		_inline void setBuffer(char *i_pBuffer, const size_t i_nSize)
		{
			this->clearBuffer();
			this->m_pBuffer = i_pBuffer;
			this->m_pPointer = i_pBuffer;
			this->m_pEnd = i_pBuffer + i_nSize;
		}
		_inline void clearBuffer()
	{
		delete[] m_pBuffer;
		this->m_pBuffer = nullptr;
		this->m_pPointer = nullptr;
		this->m_pEnd = nullptr;
	}
		_inline void skipChar()
		{
			this->m_pPointer++;
		}
	private:
		_inline size_t getBufferSize() const { return this->m_pEnd - this->m_pBuffer; }
		_inline bool checkChar(const char i_chExpect)
		{
			if (*this->m_pPointer == i_chExpect)
			{
				this->m_pPointer++;
				return true;
			}
			return false;
		}
	public:
		void parse(const std::string &i_stJson);
		std::string serialize(bool i_bFormatted);
	public:
		void clear()
		{
			this->m_oObject.clear();
		}
	public:
		_inline CObject &getObject(const std::string &i_stKey) { return this->operator[](i_stKey); }
		_inline CObject &toObject() { return this->m_oObject; }
	public:
		_inline CObject &operator[](const std::string &i_stKey)
		{
			return this->m_oObject.operator[](i_stKey);
		}
		_inline CDocument &operator=(const CDocument &i_oOther)
		{
			this->m_oObject = i_oOther.m_oObject;
			return *this;
		}
		_inline CDocument &operator=(CDocument &&i_oOther) noexcept
		{
			this->m_oObject = std::move(i_oOther.m_oObject);
			return *this;
		}
	};
}