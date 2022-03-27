#pragma once
#include "Typedef.h"

#include <fstream>
#include <string>

class CFile
{
public:
	enum EFlag
	{
		Read = std::fstream::in,
		Write = std::fstream::out,
		Append = std::fstream::app,
		Truncate = std::fstream::trunc,
		Binary = std::fstream::binary,
	};

public:
	CFile()
		: m_iFlag(EFlag::Read)
	{}

	CFile(const std::string &i_sPath, const uint i_iFlag)
	{
		this->m_sPath = i_sPath;
		this->m_iFlag = i_iFlag;
	}

	CFile(CFile &&i_oFile) noexcept
	{
		this->close();

		this->m_sPath = std::move(i_oFile.m_sPath);
		this->m_iFlag = i_oFile.m_iFlag;
		this->m_oFile = std::move(i_oFile.m_oFile);
	}

	virtual ~CFile()
	{
		this->close();
	}

public:
	bool open(const std::string &i_sPath, uint i_nFlag = Read);
	bool write(const char *i_pBuffer, size_t i_nSize) const;
	bool read(char *i_pBuffer, size_t i_nSize);
	size_t size();

public:
	bool open()
	{
		return this->open(this->m_sPath, this->m_iFlag);
	}

	void close()
	{
		return this->m_oFile.close();
	}

public:
	bool isOpen() const
	{
		return m_oFile.is_open();
	}

	bool write(const std::string &i_sBuffer) const
	{
		return this->write(i_sBuffer.c_str(), i_sBuffer.size());
	}

	std::string dirPath() const
	{
		return dirPath(this->m_sPath);
	}

	std::string name() const
	{
		return name(this->m_sPath);
	}

	std::string extension() const
	{
		return extension(this->m_sPath);
	}

public:
	bool isDirectory() const
	{
		return isDirectory(m_sPath);
	}

	bool isFile() const
	{
		return isFile(m_sPath);
	}

public:
	static bool isDirectory(const std::string &i_sPath)
	{
		return (fileType(i_sPath) & S_IFDIR) == S_IFDIR;
	}

	static bool isFile(const std::string &i_sPath)
	{
		return (fileType(i_sPath) & S_IFREG) == S_IFREG;
	}

	static std::string dirPath(const std::string &i_sFilePath)
	{
		return i_sFilePath.substr(0, i_sFilePath.find_last_of("/\\"));
	}

	static std::string name(const std::string &i_sFilePath)
	{
		return i_sFilePath.substr(i_sFilePath.find_last_of("/\\") + 1);
	}

	static std::string extension(const std::string &i_sFilePath)
	{
		return i_sFilePath.substr(i_sFilePath.find_last_of('.') + 1);
	}

	static size_t fileSize(const std::string &i_sPath);
	
	static bool exists(const std::string &i_sPath);
	static bool copyFile(const std::string &i_sInPath, const std::string &i_sOutPath);
	static bool readBinFile(const std::string &i_sPath, char *&i_pBuffer, size_t &i_nSize);

private:
	static ushort fileType(const std::string &i_sPath);

private:
	std::string		m_sPath;
	uint			m_iFlag;
	std::fstream	m_oFile;
};