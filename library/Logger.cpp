#include "Logger.h"
#include "Global.h"
#include "LockGuard.h"

#include <windows.h>
#include <iostream>
#include <ctime>

namespace Logger
{
	HANDLE g_pHandle;
	CMutex g_oMutex;

	enum EColor : byte
	{
		Black,
		DarkBlue,
		DarkGreen,
		DarkTurkeyse,
		DarkRed,
		DarkPurple,
		DarkYellow,
		DarkWhite,
		Gray,
		BrightBlue,
		BrightGreen,
		BrightTurkeyse,
		BrightRed,
		BrightPurple,
		BrightYellow,
		BrightWhite
	};
	
	std::string DateTime()
	{
		time_t nNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		char buf[80];
		tm grT;
		localtime_s(&grT, &nNow);
		strftime(buf, sizeof(buf), "%A %d %R", &grT);
		return buf;
	}

	void SetColor(const EColor i_eColor)
	{
		SetConsoleTextAttribute(g_pHandle, i_eColor);
	}

	void Output(const std::string &i_sWhat, const Logger::EColor i_eColor)
	{
		Logger::g_oMutex.lock();
		Logger::SetColor(i_eColor);
		std::cout << i_sWhat << std::endl;
		Logger::SetColor(Logger::BrightWhite);
		Logger::g_oMutex.unlock();
	}
}

// Initialize logger=======================================
void Logger::Initialize()
{
	g_pHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

// Set Title=====================================================
void Logger::SetTitle(const std::string &i_sTitle)
{
	SetConsoleTitleA(i_sTitle.c_str());
}

// Log=====================================================
void Logger::Log(const std::string &i_sMsg, ESeverity i_eSeverity)
{
	// Set string time
	std::string stOut("[" + DateTime() + "] ");

	EColor eColor = BrightWhite;

	switch (i_eSeverity)
	{
		case Fatal:
			stOut += "[Fatal]: ";
			eColor = DarkRed;
			break;
		case Error:
			stOut += "[Error]: ";
			eColor = BrightRed;
			break;
		case Warning:
			stOut += "[Warning]: ";
			eColor = BrightYellow;
			break;
		case Info:
			stOut += "[Info]: ";
			eColor = BrightTurkeyse;
			break;
		case Debug:
			stOut += "[Debug]: ";
			eColor = DarkTurkeyse;
			break;
		case Verbose:
			stOut += "[Verbose]: ";
			eColor = BrightGreen;
			break;
	}

	stOut += i_sMsg;

	Output(stOut, eColor);
}

// New Line=====================================================
void Logger::NewLine()
{
	Output("", Logger::EColor::BrightWhite);
}

// Cin Get===================================================
void Logger::WaitEnter()
{
	std::cin.get();
}