#include "JsonDocument.h"
#include "GlobalExceptionUtils.h"
#include "JsonArray.h"

Json::CDocument::CDocument()
	: m_pBuffer(nullptr), m_pPointer(nullptr), m_pEnd(nullptr)
{
}

Json::CDocument::CDocument( const std::string &i_stJson )
	: CDocument()
{
	this->parse(i_stJson);
}

Json::CDocument::~CDocument()
{
	delete[] this->m_pBuffer;
}

// Remove whitespace=======================================
void Json::CDocument::removeWhitespace()
{
	char *pBuffer = new char[this->getBufferSize()];
	size_t nNewSize = 0;
	bool bIsQuote = false;

	while (this->m_pPointer != this->m_pEnd)
	{
		const char chChar = *this->m_pPointer++;

		if (chChar == '"')
		{
			bIsQuote = !bIsQuote;
		}

		if (bIsQuote ||
			(chChar != ' ' &&
				chChar != '\n' &&
				chChar != '\r' &&
				chChar != '\t'))
		{
			pBuffer[nNewSize++] = chChar;
		}
	}

	return this->setBuffer(pBuffer, nNewSize);
}

///////////////////////////////////////////////////////////
// Parse

// Select parser===========================================
void Json::CDocument::selectParser( std::string &i_stOutBuffer, CObject *i_pObject )
{
	if (this->m_pBuffer == this->m_pEnd)
	{
		throw CBufferOutOfRangeException();
	}

	if (!i_pObject)
	{
		throw CNoBufferException();
	}

	const char chChar = *this->m_pPointer;
	switch (chChar)
	{
	case '{':
		this->skipChar();
		this->parseObject(i_stOutBuffer, i_pObject);
		break;
	case '[':
		this->parseArray(i_pObject);
		break;
	case '"':
		this->parseString(i_stOutBuffer, i_pObject);
		break;
	case 'f':
	case 't':
		this->parseBool(i_stOutBuffer, i_pObject);
		break;
	case 'n':
		this->parseNull(i_stOutBuffer, i_pObject);
		break;
	default:
		if (chChar == '-' || chChar == '.' || (chChar >= '0' && chChar <= '9'))
		{
			this->parseNumber(i_stOutBuffer, i_pObject);
		}
		else
		{
			throw CInvalidFormatException();
		}
		break;
	}
}

// Parse unicode hex=======================================
uint Json::CDocument::parseUnicodeHex()
{
	uint nRet = 0;

	if ((this->m_pEnd - this->m_pPointer) >= 4)
	{
		for (byte i = 0; i < 4; i++)
		{
			const char chChar = *this->m_pPointer;
			nRet <<= 4;
			nRet += static_cast< uint >(chChar);

			if (chChar >= '0' && chChar <= '9')
			{
				nRet -= '0';
			}
			else if (chChar >= 'A' && chChar <= 'F')
			{
				nRet -= 'A' - 10;
			}
			else if (chChar >= 'a' && chChar <= 'f')
			{
				nRet -= 'a' - 10;
			}
			else
			{
				throw CInvalidFormatException();
			}

			this->m_pPointer++;
		}
	}

	return nRet;
}

// Parse null==============================================
void Json::CDocument::parseNull( std::string &i_stOutBuffer, CObject *i_pObject )
{
	i_pObject->setType(EJsonType::JTNull);
	i_stOutBuffer.clear();
}

// Parse bool==============================================
void Json::CDocument::parseBool( std::string &i_stOutBuffer, CObject *i_pObject )
{
	i_pObject->setType(EJsonType::JTBoolean);
	i_stOutBuffer = *this->m_pPointer++;
}

// Parse object============================================
void Json::CDocument::parseObject( std::string &i_stOutBuffer, CObject *i_pObject )
{
	i_pObject->setType(EJsonType::JTObject);

	// Check empty object
	if (checkChar('}'))
	{
		return;
	}

	if (*this->m_pPointer != '"')
	{
		// Missing object name
		throw CInvalidFormatException();
	}

	while (this->m_pPointer != this->m_pEnd)
	{
		// Get key
		std::string stKey;
		this->parseString(stKey, nullptr);

		if (!checkChar(':'))
		{
			throw CInvalidFormatException();
		}

		CObject oObject;
		this->selectParser(oObject, &oObject);
		i_pObject->addObject(stKey, std::move(oObject));

		char chChar;
		do
		{
			chChar = *this->m_pPointer;
			if (checkChar('}'))
			{
				return;
			}
		}
		while (chChar != '"' && !checkChar(',') && ++this->m_pPointer != this->m_pEnd);
	}
}

// Parse string============================================
void Json::CDocument::parseString( std::string &i_stOutBuffer, CObject *i_pObject )
{
	if (i_pObject != nullptr)
	{
		i_pObject->setType(EJsonType::JTString);
	}

	char *pBegin = this->m_pPointer + 1;

	while (++this->m_pPointer != this->m_pEnd && !this->checkChar('"'))
	{
		if (static_cast< uint >(*this->m_pPointer) < 0x20)
		{
			throw CInvalidEncodingException();
		}
	}

	const size_t nSize = this->m_pPointer - pBegin;
	i_stOutBuffer = std::string(pBegin, nSize ? nSize - 1 : 0);
}

// Parse number============================================
void Json::CDocument::parseNumber( std::string &i_stOutBuffer, CObject *i_pObject )
{
	i_pObject->setType(EJsonType::JTNumber);

	char *pBegin = m_pPointer;
	char chChar;
	do
	{
		chChar = *this->m_pPointer;
	}
	while ((chChar == '-' || chChar == '.' || (chChar >= '0' && chChar <= '9')) && ++this->m_pPointer != this->m_pEnd);

	i_stOutBuffer = std::string(pBegin, this->m_pPointer - pBegin);
}

// Parse Array=============================================
void Json::CDocument::parseArray( CObject *i_pObject )
{
	i_pObject->setType(EJsonType::JTArray);
	CArray oArray;

	while (!this->checkChar(']') && ++this->m_pPointer != this->m_pEnd)
	{
		if (checkChar(']'))
			return;

		CObject oObject;
		this->selectParser(oObject, &oObject);
		oArray.push_back(std::move(oObject));

		if (*this->m_pPointer != ',' && *this->m_pPointer != ']')
		{
			throw CInvalidFormatException();
		}
	}

	*i_pObject = oArray;
}

///////////////////////////////////////////////////////////
// Serialize

// Select serializer=======================================
void Json::CDocument::selectSerializer( std::string &i_stBuffer, CObject &i_oObject, const bool i_bFormatted)
{
	switch (i_oObject.type())
	{
	case EJsonType::JTObject:
		CDocument::serializeObject(i_stBuffer, i_oObject, i_bFormatted);
		break;
	case EJsonType::JTNumber:
		CDocument::serializeNumber(i_stBuffer, i_oObject);
		break;
	case EJsonType::JTBoolean:
		CDocument::serializeBool(i_stBuffer, i_oObject);
		break;
	case EJsonType::JTString:
		CDocument::serializeString(i_stBuffer, i_oObject);
		break;
	case EJsonType::JTArray:
		CDocument::serializeArray(i_stBuffer, i_oObject, i_bFormatted);
		break;
	case EJsonType::JTNull:
		CDocument::serializeNull(i_stBuffer);
		break;
	default: break;
	}
}

// Serialize array=========================================
void Json::CDocument::serializeArray( std::string &i_stBuffer, CObject &i_oObject, const bool i_bFormatted)
{
	i_stBuffer +=  i_bFormatted ? "[\r\n    " : "[";

	CArray oArray = i_oObject.toArray();
	
	bool bIsFirst = true;
	for (auto &it : oArray)
	{
		if (!bIsFirst)
		{
			i_stBuffer += i_bFormatted ? ",\r\n    " : ",";
		}
		else
		{
			bIsFirst = false;
		}

		CDocument::selectSerializer(i_stBuffer, const_cast<CObject &>(it), i_bFormatted);
	}

	i_stBuffer += i_bFormatted ?  "\r\n]" : "]";
}

// Serialize object========================================
void Json::CDocument::serializeObject( std::string &i_stBuffer, CObject &i_oObject, const bool i_bFormatted)
{
	i_stBuffer += i_bFormatted ? "{\r\n    " : "{";

	bool bIsFirst = true;
	for (const std::string &stItem : i_oObject.getKeys())
	{
		if (!bIsFirst)
		{
			i_stBuffer += i_bFormatted ? ",\r\n    " : ",";
		}
		else
		{
			bIsFirst = false;
		}

		i_stBuffer += '"';
		i_stBuffer += stItem;
		i_stBuffer += i_bFormatted ? "\": " : "\":";

		CObject &oObject = i_oObject.operator[](stItem);
		CDocument::selectSerializer(i_stBuffer, oObject, i_bFormatted);
	}

	i_stBuffer += i_bFormatted ? "\r\n}" : "}";
}

// Serialize number========================================
void Json::CDocument::serializeNumber( std::string &i_stBuffer, CObject &i_oObject )
{
	i_stBuffer += i_oObject;
}

// Serialize string========================================
void Json::CDocument::serializeString( std::string &i_stBuffer, CObject &i_oObject )
{
	i_stBuffer += '"';
	i_stBuffer += i_oObject;
	i_stBuffer += '"';
}

// Serialize null==========================================
void Json::CDocument::serializeNull( std::string &i_stBuffer )
{
	i_stBuffer += "null";
}

// Serialize bool==========================================
void Json::CDocument::serializeBool( std::string &i_stBuffer, CObject &i_oObject )
{
	if (i_oObject.empty() ||
		i_oObject.at(0) < '1' ||
		i_oObject.at(0) == 'f' ||
		i_oObject.at(0) == 'F')
	{
		i_stBuffer += "false";
	}
	else
	{
		i_stBuffer += "true";
	}
}

// Parse===================================================
void Json::CDocument::parse( const std::string &i_stJson )
{
	this->m_oObject.clear();
	const size_t nBufferSize = i_stJson.size();
	char *pBuffer = new char[nBufferSize];
	memcpy(pBuffer, i_stJson.c_str(), nBufferSize);
	this->setBuffer(pBuffer, nBufferSize);

	this->removeWhitespace();

	if (*this->m_pPointer != '{' && *this->m_pPointer != '[')
	{
		throw CInvalidFormatException();
	}

	try
	{
		this->selectParser(this->m_oObject, &this->m_oObject);
	}
	catch (const CInvalidFormatException &e)
	{
		this->clearBuffer();
		throw e;
	}

	this->clearBuffer();
}

std::string Json::CDocument::serialize(const bool i_bFormatted = false)
{
	std::string stRet;
	CDocument::selectSerializer(stRet, this->m_oObject, i_bFormatted);
	return stRet;
}