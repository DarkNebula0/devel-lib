#pragma once
#include <string>
#include "Typedef.h"

namespace Logger
{
	enum ESeverity : byte
	{
		None,
		Fatal,
		Error,
		Warning,
		Info,
		Debug,
		Verbose,
	};

	void Initialize();
	void SetTitle(const std::string &i_sTitle);
	void Log(const std::string &i_sMsg, ESeverity i_eSeverity = None);

	void NewLine();
	void WaitEnter();

	__forceinline void LogDebug(const std::string &i_sMsg, const ESeverity i_eSeverity = None)
	{
		#ifdef _DEBUG
		return Log("[Dbg]: " + i_sMsg, i_eSeverity);
		#endif
	}
}

#pragma region Log Always
#define LOG_IF(msg,severity,cond)	if (cond) Logger::Log(msg, severity)

#define LOG_NONE(x)		Logger::Log(x, Logger::ESeverity::None)
#define LOG_FATAL(x)	Logger::Log(x, Logger::ESeverity::Fatal)
#define LOG_ERROR(x)	Logger::Log(x, Logger::ESeverity::Error)
#define LOG_WARNING(x)	Logger::Log(x, Logger::ESeverity::Warning)
#define LOG_INFO(x)		Logger::Log(x, Logger::ESeverity::Info)
#define LOG_DEBUG(x)	Logger::Log(x, Logger::ESeverity::Debug)
#define LOG_VERBOSE(x)	Logger::Log(x, Logger::ESeverity::Verbose)

#define LOG_NONE_IF(msg,cond)		LOG_IF(msg,Logger::ESeverity::None,cond)
#define LOG_FATAL_IF(msg,cond)		LOG_IF(msg,Logger::ESeverity::Fatal,cond)
#define LOG_ERROR_IF(msg,cond)		LOG_IF(msg,Logger::ESeverity::Error,cond)
#define LOG_WARNING_IF(msg,cond)	LOG_IF(msg,Logger::ESeverity::Warning,cond)
#define LOG_INFO_IF(msg,cond)		LOG_IF(msg,Logger::ESeverity::Info,cond)
#define LOG_DEBUG_IF(msg,cond)		LOG_IF(msg,Logger::ESeverity::Debug,cond)
#define LOG_VERBOSE_IF(msg,cond)	LOG_IF(msg,Logger::ESeverity::Verbose,cond)

#define LOGN(x) LOG_NONE(x)
#define LOGF(x) LOG_FATAL(x)
#define LOGE(x) LOG_ERROR(x)
#define LOGW(x) LOG_WARNING(x)
#define LOGI(x) LOG_INFO(x)
#define LOGD(x) LOG_DEBUG(x)
#define LOGV(x) LOG_VERBOSE(x)

#define LOGN_IF(msg,cond) LOG_NONE_IF(msg,cond)
#define LOGF_IF(msg,cond) LOG_FATAL_IF(msg,cond)
#define LOGE_IF(msg,cond) LOG_ERROR_IF(msg,cond)
#define LOGW_IF(msg,cond) LOG_WARNING_IF(msg,cond)
#define LOGI_IF(msg,cond) LOG_INFO_IF(msg,cond)
#define LOGD_IF(msg,cond) LOG_DEBUG_IF(msg,cond)
#define LOGV_IF(msg,cond) LOG_VERBOSE_IF(msg,cond)
#pragma endregion

#pragma region Log Debug
#define LOG_IF_DEBUG(msg,severity,cond)	if (cond) Logger::LogDebug(msg, severity)

#define LOG_NONE_DEBUG(x)		Logger::LogDebug(x, Logger::ESeverity::None)
#define LOG_FATAL_DEBUG(x)		Logger::LogDebug(x, Logger::ESeverity::Fatal)
#define LOG_ERROR_DEBUG(x)		Logger::LogDebug(x, Logger::ESeverity::Error)
#define LOG_WARNING_DEBUG(x)	Logger::LogDebug(x, Logger::ESeverity::Warning)
#define LOG_INFO_DEBUG(x)		Logger::LogDebug(x, Logger::ESeverity::Info)
#define LOG_DEBUG_DEBUG(x)		Logger::LogDebug(x, Logger::ESeverity::Debug)
#define LOG_VERBOSE_DEBUG(x)	Logger::LogDebug(x, Logger::ESeverity::Verbose)

#define LOG_NONE_IF_DEBUG(msg,cond)			LOG_IF_DEBUG(msg,Logger::ESeverity::None,cond)
#define LOG_FATAL_IF_DEBUG(msg,cond)		LOG_IF_DEBUG(msg,Logger::ESeverity::Fatal,cond)
#define LOG_ERROR_IF_DEBUG(msg,cond)		LOG_IF_DEBUG(msg,Logger::ESeverity::Error,cond)
#define LOG_WARNING_IF_DEBUG(msg,cond)		LOG_IF_DEBUG(msg,Logger::ESeverity::Warning,cond)
#define LOG_INFO_IF_DEBUG(msg,cond)			LOG_IF_DEBUG(msg,Logger::ESeverity::Info,cond)
#define LOG_DEBUG_IF_DEBUG(msg,cond)		LOG_IF_DEBUG(msg,Logger::ESeverity::Debug,cond)
#define LOG_VERBOSE_IF_DEBUG(msg,cond)		LOG_IF_DEBUG(msg,Logger::ESeverity::Verbose,cond)

#define LOGN_DEBUG(x) LOG_NONE_DEBUG(x)
#define LOGF_DEBUG(x) LOG_FATAL_DEBUG(x)
#define LOGE_DEBUG(x) LOG_ERROR_DEBUG(x)
#define LOGW_DEBUG(x) LOG_WARNING_DEBUG(x)
#define LOGI_DEBUG(x) LOG_INFO_DEBUG(x)
#define LOGD_DEBUG(x) LOG_DEBUG_DEBUG(x)
#define LOGV_DEBUG(x) LOG_VERBOSE_DEBUG(x)

#define LOGN_IF_DEBUG(msg,cond) LOG_NONE_IF_DEBUG(msg,cond)
#define LOGF_IF_DEBUG(msg,cond) LOG_FATAL_IF_DEBUG(msg,cond)
#define LOGE_IF_DEBUG(msg,cond) LOG_ERROR_IF_DEBUG(msg,cond)
#define LOGW_IF_DEBUG(msg,cond) LOG_WARNING_IF_DEBUG(msg,cond)
#define LOGI_IF_DEBUG(msg,cond) LOG_INFO_IF_DEBUG(msg,cond)
#define LOGD_IF_DEBUG(msg,cond) LOG_DEBUG_IF_DEBUG(msg,cond)
#define LOGV_IF_DEBUG(msg,cond) LOG_VERBOSE_IF_DEBUG(msg,cond)
#pragma endregion
