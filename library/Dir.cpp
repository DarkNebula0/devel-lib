#include "Dir.h"
#include "File.h"
#include <Windows.h>


bool CDir::reload()
{
	this->m_asFiles.clear();

	if (this->exists() && this->isDirectory())
	{
		WIN32_FIND_DATAA oData;

		const HANDLE hHandle = FindFirstFileA((this->m_sPath + "\\*.*").c_str(), &oData);
		if (hHandle != INVALID_HANDLE_VALUE)
		{
			FindNextFileA(hHandle, &oData);

			while (FindNextFileA(hHandle, &oData))
			{
				this->m_asFiles.push_back(oData.cFileName);
			}

			FindClose(hHandle);

			return true;
		}
	}

	return false;
}

bool CDir::isDirectory() const
{
	return CFile::isDirectory(this->m_sPath);
}

bool CDir::exists() const
{
	return CFile::exists(this->m_sPath);
}


CFile CDir::openFile(const std::string &i_sName, const uint i_iFlag) const
{
	return CFile(this->fileFullPath(i_sName), i_iFlag);
}


bool CDir::makeDir(const std::string &i_sName) const
{
	if (this->m_sPath.empty())
	{
		return false;
	}

	return this->makePath(this->m_sPath + i_sName);
}

bool CDir::makePath(std::string i_sPath)
{
	if (i_sPath.empty())
	{
		return false;
	}

	CDir::addTrailingSlash(i_sPath);

	return CreateDirectoryA(i_sPath.data(), nullptr);
}