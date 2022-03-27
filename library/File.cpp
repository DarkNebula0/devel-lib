#include "File.h"


bool CFile::open(const std::string &i_sPath, const uint i_nFlag)
{
	this->m_oFile.open(i_sPath.data(), i_nFlag);
	return this->isOpen();
}

bool CFile::write(const char *i_pBuffer, const size_t i_nSize) const
{
	if (this->isOpen())
	{
		return (this->m_oFile.rdbuf()->sputn(i_pBuffer, i_nSize) > 0);
	}

	return false;
}

bool CFile::read(char *i_pBuffer, const size_t i_nSize)
{
	if (this->isOpen())
	{
		return this->m_oFile.read(i_pBuffer, i_nSize).good();
	}

	return false;
}


size_t CFile::size()
{
	if (!this->isOpen())
	{
		return CFile::fileSize(this->m_sPath);
	}

	// Get old pos
	const std::streampos fOldPos = this->m_oFile.tellg();
	this->m_oFile.seekg(0);

	std::streampos fSize = this->m_oFile.tellg();
	this->m_oFile.seekg(0, std::fstream::end);

	fSize = this->m_oFile.tellg() - fSize;
	this->m_oFile.seekg(fOldPos);

	return static_cast<size_t>(fSize);
}

bool CFile::exists(const std::string &i_sPath)
{
	struct stat oStatus = {};
	return (stat(i_sPath.data(), &oStatus) == 0);
}

size_t CFile::fileSize(const std::string &i_sPath)
{
	return static_cast<size_t>(std::ifstream(i_sPath.data(), std::ios::binary | std::ios::ate).tellg());
}

bool CFile::copyFile(const std::string &i_sInPath, const std::string &i_sOutPath)
{
	const size_t nSize = CFile::fileSize(i_sInPath);
	std::ifstream oIn(i_sInPath.data(), std::fstream::binary | std::fstream::in);
	std::ofstream oOut(i_sOutPath.data(), std::fstream::binary | std::fstream::out | std::fstream::trunc);
	
	bool fReturn = false;
	
	if (nSize > 0 && oIn.is_open() && oOut.is_open())
	{
		// Read
		char *pBuffer = new char[nSize];
	
		if (oIn.read(pBuffer, nSize).good() && oOut.rdbuf()->sputn(pBuffer, nSize) == nSize)
		{
			fReturn = true;
		}

		delete[] pBuffer;
	}

	oIn.close();
	oOut.close();

	return fReturn;
}

bool CFile::readBinFile(const std::string &i_sPath, char *&i_pBuffer, size_t &i_nSize)
{
	CFile oFile(i_sPath, EFlag::Binary | EFlag::Read);
	if (oFile.open())
	{
		i_nSize = oFile.size();
		i_pBuffer = new char[i_nSize];
		if (oFile.read(i_pBuffer, i_nSize))
		{
			oFile.close();
			return true;
		}

		delete[] i_pBuffer;
		oFile.close();
	}

	i_pBuffer = nullptr;
	i_nSize = 0;
	return false;
}

ushort CFile::fileType(const std::string &i_sPath)
{
	struct stat oStatus = {};
	stat(i_sPath.c_str(), &oStatus);

	return oStatus.st_mode;
}