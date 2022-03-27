#pragma once
#include <cstdlib>
#include <string>
#include <random>


namespace Utils
{
	inline std::string RepeatString(const std::string &i_sString, size_t i_nTimes)
	{
		std::string sString = i_sString;

		for (; i_nTimes > 1; i_nTimes--)
		{
			sString += i_sString;
		}

		return sString;
	}


	// When you don't pass a length(means -1), the string has to be zero terminated!
	inline std::vector<std::string> Split(const char *i_szString, size_t i_nLength, const char i_bDelimeter, const size_t i_nReserve = -1)
	{
		std::vector<std::string> aSplitted;

		if (*i_szString != 0x00)
		{
			if (i_nLength == size_t(-1))
				i_nLength = strlen(i_szString);

			aSplitted.reserve((i_nReserve != size_t(-1) ? i_nReserve : std::count(i_szString, i_szString + i_nLength, i_bDelimeter) + 1));

			size_t nLastPos = 0;
			for (size_t i = 0; i < i_nLength; i++)
			{
				if (i_szString[i] == i_bDelimeter)
				{
					aSplitted.emplace_back(i_szString + nLastPos, i - nLastPos);
					nLastPos = i + 1;
				}
			}
			aSplitted.emplace_back(i_szString + nLastPos, i_nLength - nLastPos);
		}

		return aSplitted;
	}

	inline std::vector<std::string> Split(const std::string &i_sString, const char i_bDelimeter, const size_t i_nReserve = -1)
	{
		return Split(i_sString.c_str(), i_sString.length(), i_bDelimeter, i_nReserve);
	}



	inline std::string Join(const std::vector<std::string> &i_asString, const std::string &i_sDelimeter)
	{
		std::string sData = "";

		for (const std::string &sString : i_asString)
		{
			if (!sData.empty())
			{
				sData += i_sDelimeter;
			}

			sData += sString;
		}

		return sData;
	}

	inline std::string Join(const std::vector<std::string> &i_asString, const char i_bDelimeter)
	{
		return Join(i_asString, std::string(&i_bDelimeter, 1));
	}



	inline std::string &Replace(std::string &i_sSource, const char i_bFind, const char i_bReplaceWith)
	{
		const size_t iSize = i_sSource.size();
		for (size_t i = 0; i < iSize; i++)
		{
			if (i_sSource[i] == i_bFind)
				i_sSource[i] = i_bReplaceWith;
		}

		return i_sSource;
	}

	inline std::string Replace(const std::string &i_sSource, const std::string &i_sFind, const std::string &i_sReplaceWith)
	{
		std::string sReturn;

		size_t nPos = i_sSource.find(i_sFind);
		size_t nCurrentPos = 0;
		while (nPos != std::string::npos)
		{
			sReturn += i_sSource.substr(nCurrentPos, nPos);
			sReturn += i_sReplaceWith;

			nCurrentPos = nPos + i_sFind.size();
			nPos = i_sSource.find(i_sSource, nPos);
		}

		if (nCurrentPos != i_sSource.size())
			sReturn += i_sSource.substr(nCurrentPos, i_sSource.size());

		return sReturn;
	}



	inline long long StringToInt64(const std::string &i_sBuffer)
	{
		char *pEnd;
		return strtoll(i_sBuffer.c_str(), &pEnd, 10);
	}

	inline unsigned long long StringToUInt64(const std::string &i_sBuffer)
	{
		return static_cast<unsigned long long>(StringToInt64(i_sBuffer));
	}

	inline int StringToInt(const std::string &i_sBuffer)
	{
		char *pEnd;
		return strtol(i_sBuffer.c_str(), &pEnd, 10);
	}

	inline unsigned int StringToUInt(const std::string &i_sBuffer)
	{
		return static_cast<unsigned int>(StringToInt(i_sBuffer));
	}

	inline float StringToFloat(const std::string & i_stBuffer)
	{
		char *pEndPtr;
		return std::strtof(i_stBuffer.c_str(), &pEndPtr);
	}

	inline double StringToDouble(const std::string &i_stBuffer)
	{
		char *pEndPtr;
		return std::strtod(i_stBuffer.c_str(), &pEndPtr);
	}


	inline std::string StringToLowerCopy(const std::string &i_sBuffer)
	{
		std::string sToLower = i_sBuffer;
		for (char &c : sToLower)
			c = tolower(c);
		return sToLower;
	}

	inline void StringToLower(std::string &i_sBuffer)
	{
		for (char &c : i_sBuffer)
			c = tolower(c);
	}



	inline std::string StringToUpperCopy(const std::string &i_sBuffer)
	{
		std::string sToLower = i_sBuffer;
		for (char &c : sToLower)
			c = toupper(c);
		return sToLower;
	}

	inline void StringToUpper(std::string &i_sBuffer)
	{
		for (char &c : i_sBuffer)
			c = toupper(c);
	}



	inline bool CheckName(const char *i_pString, const size_t i_nSize)
	{
		for (size_t i = 0; i < i_nSize; i++)
		{
			if ((i_pString[i] < '0' || i_pString[i] > '9') &&
				(i_pString[i] < 'a' || i_pString[i] > 'z') &&
				(i_pString[i] < 'A' || i_pString[i] > 'Z') &&
				(i_pString[i] != 'Ä') && (i_pString[i] != 'ä') &&
				(i_pString[i] != 'Ü') && (i_pString[i] != 'ü') &&
				(i_pString[i] != 'Ö') && (i_pString[i] != 'ö') &&
				(i_pString[i] != 'ß') && (i_pString[i] != '*') &&
				(i_pString[i] != '>') && (i_pString[i] != '<') &&
				(i_pString[i] != '?') && (i_pString[i] != '°') &&
				(i_pString[i] != '!') && (i_pString[i] != '"') &&
				(i_pString[i] != '§') && (i_pString[i] != '$') &&
				(i_pString[i] != '=') && (i_pString[i] != '@') &&
				(i_pString[i] != '_') && (i_pString[i] != '~'))
			{
				return false;
			}
		}
		return true;
	}

	inline bool CheckName(const std::string &i_sData)
	{
		return CheckName(i_sData.c_str(), i_sData.size());
	}



	inline bool SearchInStr(const std::string &i_sString, const std::string &i_sToFind, const bool i_fCaseSensetive = true)
	{
		const size_t nPosition = (i_fCaseSensetive ? i_sString.find(i_sToFind) : StringToLowerCopy(i_sString).find(StringToLowerCopy(i_sToFind)));

		return (nPosition != std::string::npos);
	}



	inline size_t StringSize(const char *i_szString, const size_t i_nMaxSize)
	{
		size_t nSize = 0;

		if (i_szString)
		{
			for (; nSize < i_nMaxSize && *i_szString++ != 0x00; nSize++) {}
		}

		return nSize;
	}



	inline void SetString(char *i_pDestination, const char *i_szString, const size_t i_nSize, size_t i_nMax)
	{
		memset(static_cast<void *>(i_pDestination), 0, i_nMax);

		if (i_nSize <= i_nMax)
		{
			i_nMax = i_nSize;
		}

		memcpy(static_cast<void *>(i_pDestination), i_szString, i_nMax);
	}

	inline void SetString(char *i_pDestination, const char *i_szString, const size_t i_nMax)
	{
		SetString(i_pDestination, i_szString, strlen(i_szString), i_nMax);
	}

	inline void SetString(char *i_pDestination, const std::string &i_sData, const size_t i_nMax)
	{
		SetString(i_pDestination, i_sData.c_str(), i_sData.length(), i_nMax);
	}
	
	inline std::string GetString(char *i_pSource, const size_t i_nMaxSize)
	{
		const size_t nSize = StringSize(i_pSource, i_nMaxSize);
		return std::string(i_pSource, nSize);
	}



	inline size_t CountCharacters(const char *i_szString, const size_t i_nSize, const char i_bDelimeter)
	{
		return std::count(i_szString, i_szString + i_nSize, i_bDelimeter);
	}

	inline size_t CountCharacters(const std::string &i_sString, const char i_bDelimeter)
	{
		return CountCharacters(i_sString.c_str(), i_sString.size(), i_bDelimeter);
	}



	// E.g.: std::string sString = "Hallo"; char szData[10]; memset(szData, 0, sizeof(szData)); StringToArray(szData, sString);
	template <typename T>
	void StringToArray(T &i_pDestination, const char *i_szString, const size_t i_nSize)
	{
		return SetString(i_pDestination, i_szString, i_nSize, sizeof(i_pDestination));
	}

	template <typename T>
	void StringToArray(T &i_pDestination, const char *i_szString)
	{
		return StringToArray(i_pDestination, i_szString, strlen(i_szString));
	}

	template <typename T>
	void StringToArray(T &i_pDestination, const std::string &i_sData)
	{
		return StringToArray(i_pDestination, i_sData.c_str(), i_sData.length());
	}



	// E.g.: char szData[32]; std::string sString = ArrayToString(szData);
	template <typename T>
	std::string ArrayToString(const T &i_pSource)
	{
		return GetString(const_cast<char *>(i_pSource), sizeof(i_pSource));
	}

	template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
	inline std::string ToHexString(T i_oValue)
	{
		char achHexString[4 * sizeof(T) + 3];
		sprintf_s(achHexString, sizeof(achHexString), "0x%02x", i_oValue);
		return std::string(achHexString);
	}
}