#include "IniFile.h"
#include "File.h"

#include <Windows.h>


int CIniFile::readInt(const std::string &i_sSection, const std::string &i_sKey, const int i_nDefaultValue) const
{
	return GetPrivateProfileIntA(i_sSection.c_str(), i_sKey.c_str(), i_nDefaultValue, this->m_sPath.c_str());
}

std::string CIniFile::readString(const std::string &i_sSection, const std::string &i_sKey, const std::string &i_sDefault) const
{
	constexpr uint READSTRING_SIZE = 10000;
	char szBuffer[READSTRING_SIZE + 1];
	memset(szBuffer, 0, sizeof(szBuffer));

	const size_t nSize = GetPrivateProfileStringA(i_sSection.c_str(), i_sKey.c_str(), "", szBuffer, READSTRING_SIZE, this->m_sPath.c_str());
	std::string sString = (GetLastError() != ERROR_SUCCESS || nSize == 0 ? i_sDefault : std::string(szBuffer, nSize));
	
	return sString;
}

bool CIniFile::writeString(const std::string &i_sSection, const std::string &i_sKey, const std::string &i_sBuffer) const
{
	return (WritePrivateProfileStringA(i_sSection.c_str(), i_sKey.c_str(), i_sBuffer.c_str(), this->m_sPath.c_str()) > 0);
}

std::vector<std::string> CIniFile::readSections() const
{
	std::vector<std::string> asSections;

	const size_t nFullSize(CFile::fileSize(this->m_sPath));
	if (nFullSize > 0) {
		char *szBuffer = new char[nFullSize + 1];

		const size_t nSize = GetPrivateProfileSectionNamesA(szBuffer, static_cast<DWORD>(nFullSize), this->m_sPath.c_str());

		if (GetLastError() == ERROR_SUCCESS && nSize > 0) {
			for (size_t i = 0; i < nSize; i += strlen(szBuffer + i) + 1) {
				asSections.emplace_back(szBuffer + i);
			}
		}

		delete[] szBuffer;
	}

	return asSections;
}

std::vector<std::string> CIniFile::readKeys(const std::string &i_sSection) const
{
	std::vector<std::string> asKeys;

	const size_t nFullSize(CFile::fileSize(this->m_sPath));
	if (nFullSize > 0) {
		char *szBuffer = new char[nFullSize + 1];

		const size_t nSize = GetPrivateProfileSectionA(i_sSection.c_str(), szBuffer, static_cast<DWORD>(nFullSize), this->m_sPath.c_str());

		if (GetLastError() == ERROR_SUCCESS && nSize > 0) {
			for (size_t i = 0; i < nSize; i += strlen(szBuffer + i) + 1) {
				asKeys.emplace_back(szBuffer + i);
			}
		}

		delete[] szBuffer;
	}

	return asKeys;
}