#pragma once
#include <string>
#include <vector>
#include "Typedef.h"

class CDir
{
public:
	CDir(const std::string &i_sPath)
	{
		this->setPath(i_sPath);
	}
	virtual ~CDir() = default;

public:
	auto begin() const
	{
		return this->m_asFiles.begin();
	}
	auto end() const
	{
		return this->m_asFiles.end();
	}

public:
	void setPath(const std::string &i_sPath)
	{
		this->m_sPath = i_sPath;
		
		CDir::addTrailingSlash(this->m_sPath);

		if (this->exists())
		{
			this->reload();
		}
	}
	bool reload();
	bool makeDir(const std::string &i_sName) const;

public:
	CFile openFile(const std::string &i_sName, const uint i_iFlag) const;

	std::string fileFullPath(const std::string &i_sName) const
	{
		return this->m_sPath + i_sName;
	}

public:
	uint fileCount() const
	{
		return static_cast<uint>(this->m_asFiles.size());
	}
	
	const std::string &path() const
	{
		return this->m_sPath;
	}

public:
	bool isDirectory() const;
	bool exists() const;

public:
	static bool makePath(std::string i_sPath);

private:
	static void addTrailingSlash(std::string &i_sPath)
	{
		if (*(--i_sPath.end()) != '\\')
		{
			i_sPath += "\\";
		}
	}

private:
	std::string					m_sPath;
	std::vector<std::string>	m_asFiles;
};