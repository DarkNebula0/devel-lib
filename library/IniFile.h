#pragma once
#include "GlobalStringUtils.h"
#include "Typedef.h"
#include "Global.h"

#include <type_traits>
#include <typeinfo>
#include <string>
#include <vector>
#include <atomic>

#define IniDefaultTimeStr			"1s"
#define IniReadTime(iniVar, saveVar, key)	\
								{	std::string sTime = iniVar.readString(key, DefaultTimeStr);													\
									if (sTime.size() <= 1) sTime = DefaultTimeStr;																\
									std::string sTimeResolution(sTime.substr(sTime.size() - 2));												\
									if (sTimeResolution == "ms")																				\
										saveVar = static_cast<uint>(sTime.substr(0, sTime.size() - 2));											\
									else {																										\
										if (sTimeResolution[1] == 's')																			\
											saveVar = SECONDS(static_cast<uint>(sTime.substr(0, sTime.size() - 1)));							\
										else if (sTimeResolution[1] == 'm')																		\
											saveVar = MINUTES(static_cast<uint>(sTime.substr(0, sTime.size() - 1)));							\
										else if (sTimeResolution[1] == 'h')																		\
											saveVar = HOURS(static_cast<uint>(sTime.substr(0, sTime.size() - 1)));								\
										else {																									\
											return false;																						\
										}																										\
									}																											\
								}
#define IniReadFromTil(iniVar, saveArr, key, convert)	\
								{	std::vector<std::string> asFromTil = split(iniVar.readString(key, "1-1"), '-', 2);							\
									if(asFromTil.size() != 2) {																					\
										return false;																							\
									}																											\
									saveArr[0] = convert(asFromTil[0]);																			\
									saveArr[1] = convert(asFromTil[1]);																			\
								}

#define IniReadEndpoint(iniVar, endPoint, section)		\
								{																												\
									endPoint.sIp	= iniVar.readString(section, "IP", "");														\
									endPoint.wPort	= iniVar.read<unsigned short>(section, "Port", 0);											\
								}
#define IniReadDatabaseConnectionData(iniVar, connectionData, section)		\
								{																												\
									connectionData.sHost			= iniVar.readString(section, "Host", "");									\
									connectionData.sUsername		= iniVar.readString(section, "Username", "");								\
									connectionData.sPassword		= iniVar.readString(section, "Password", "");								\
									connectionData.sDatabase		= iniVar.readString(section, "Database", "");								\
									connectionData.wPort			= iniVar.read<ushort>(section, "Port", 3306);								\
									connectionData.fAutoReconnect	= iniVar.read<bool>(section, "AutoReconnect", true);						\
								}

class CIniFile
{
public:
	CIniFile(std::string &&i_sPath)
		: m_sPath(i_sPath)
	{ }

public:
	void beginSection(const std::string &i_sSection)
	{
		this->m_sSection = i_sSection;
	}
	const std::string &section() const
	{
		return this->m_sSection;
	}
	void endSection()
	{
		this->m_sSection = "";
	}

private:
	int readInt(const std::string &i_sSection, const std::string &i_sKey, int i_nDefaultValue = 0) const;
	float readFloat(const std::string &i_sSection, const std::string &i_sKey, const float i_rDefault) const
	{
		std::string sValue = this->readString(i_sSection, i_sKey, "E");
		return (sValue == "E" ? i_rDefault :  Utils::StringToFloat(sValue));
	}
	double readDouble(const std::string &i_sSection, const std::string &i_sKey, const double i_rDefault) const
	{
		std::string sValue = this->readString(i_sSection, i_sKey, "E");
		return (sValue == "E" ? i_rDefault : Utils::StringToFloat(sValue));
	}
	int64 readInt64(const std::string &i_sSection, const std::string &i_sKey, const int64 i_nDefault = 0) const
	{
		return Utils::StringToInt64(this->readString(i_sSection, i_sKey, std::to_string(i_nDefault)));
	}

public:
	std::string readString(const std::string &i_sSection, const std::string &i_sKey, const std::string &i_sDefault) const;
	std::string readString(const std::string &i_sKey, const std::string &i_sDefault) const
	{
		return this->readString(this->section(), i_sKey, i_sDefault);
	}

	bool writeString(const std::string &i_sSection, const std::string &i_sKey, const std::string &i_sBuffer) const;
	bool writeString(const std::string &i_sKey, const std::string &i_sBuffer) const
	{
		return this->writeString(this->section(), i_sKey, i_sBuffer);
	}

public:
	std::vector<std::string> readSections() const;
	std::vector<std::string> readKeys(const std::string &i_sSection) const;

public:
	template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
	bool write(const std::string &i_sSection, const std::string &i_sKey, const T &i_oValue)
	{
		return this->writeString(i_sSection, i_sKey, std::to_string(i_oValue));
	}

	template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
	bool write(const std::string &i_sKey, const T &i_oValue)
	{
		return this->writeString(this->m_sSection, i_sKey, std::to_string(i_oValue));
	}

#pragma warning( push )
#pragma warning( disable : 4244 4800)
	template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
	T read(const std::string &i_sSection, const std::string &i_sKey, const T &i_tDefault = 0)
	{
		if (std::is_same_v<T, float>)
		{
			return this->readFloat(i_sSection, i_sKey, i_tDefault);
		}
		else if (std::is_same_v<T, double>)
		{
			return this->readDouble(i_sSection, i_sKey, i_tDefault);
		}
		else if (sizeof(i_tDefault) == 8)
		{
			return static_cast<T>(this->readInt64(i_sSection, i_sKey, static_cast<int64>(i_tDefault)));
		}

		return static_cast<T>(this->readInt(i_sSection, i_sKey, static_cast<int>(i_tDefault)));
	}

	template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
	T read(const std::string &i_sKey, const T &i_tDefault = 0)
	{
		return this->read<T>(this->m_sSection, i_sKey, i_tDefault);
	}
#pragma warning( pop )

private:
	std::string m_sSection;
	const std::string m_sPath;
};