#pragma once
#include <string>
#include "GlobalStringUtils.h"
#include "GlobalRandomUtils.h"
#include "GlobalExceptionUtils.h"

#define SIZEOF_ARRAY(x)				(sizeof(x) / sizeof(x[0]))

#define CLEAR_MEMORY(x)				memset(x, 0, sizeof(x))

#define SECONDS(x)					(x * 1000)
#define MINUTES(x)					SECONDS(x * 60)
#define HOURS(x)					MINUTES(x * 60)

#define THREAD_DELAY_MS 3
#define THREAD_SLEEP() (std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS)))

#define CastShared(type, shared)	std::static_pointer_cast<type>(shared)

#define CatUniqueVar_(x,y)		x##y
#define CatUniqueVar(x,y)		CatUniqueVar_(x,y)
#define RecursiveLockGuard(x)	CLockGuard CatUniqueVar(locker, __COUNTER__)(x);


// Network
#define TCP_SERVER_BUFFER_SIZE_DEFAULT		512
#define TCP_SERVER_MAX_CONNECTIONS_DEFAULT	10000
#define TCP_SERVER_THREADS_DEFAULT			4

namespace Network
{
	struct SEndpoint
	{
		std::string sIp;
		unsigned short wPort;

		SEndpoint()
			: wPort(0)
		{
		}
	};

	enum EError : unsigned char
	{
		ESuccess,
		// Everything is OK
		ENoSocket,
		// Create socket and try again
		EIPFormat,
		// Wrong IP Format
		EWSALastError,
		// Use WSAGetLastError function
		ETimeout,
		// Couldn't connect timeout
		EProtocolNotFound,
	};

	struct SProtocolData
	{
		int iType;
		int iProtocol;

		SProtocolData()
			: iType(-1), iProtocol(-1)
		{}

		bool isGood() const
		{
			return (iType != -1 && iProtocol != -1);
		}
	};

	enum ESocketProtocol : unsigned char
	{
		SPTCP,
		SPUDP,
	};

	enum ERecvFlag : unsigned char
	{
		RFStandard = 0,
		RFOOB = 1 << 0,
		RFPeek = 1 << 1,
		RFWaitAll = 1 << 3,
	};
	enum ESendFlag : unsigned char
	{
		SFStandard = 0,
		SFOOB = 1 << 0,
		SFWaitAll = 1 << 3,
	};

	namespace Web
	{
		enum EProtocol : unsigned char
		{
			ENothing,
			EHttp,
			EHttps,
			EWs,
			EWss,
		};

		enum EMethods: unsigned char
		{
			EGet,
			EHead,
			EPost,
			EPut,
			EDelete,
			EConnect,
			EOptions,
			EPatch,
		};

		enum ERequestError
		{
			ESuccess,
			ETimeout,
			EInvalidHost,
			EInvalidMethode,
			ESocketError,
			EAlreadyConnected,
			EConnectionLost,
		};
	}
}

// MySQL
namespace MySQL
{
	enum EError : unsigned char
	{
		ENothing,
		// Nothing
		ESuccess,
		// Everything is OK
		ENotInitializable,
		// Cannot initialize mysql_init
		EAlreadyOpen,
		// Close before open
		ELastError,
		// Use lastError function
		ENotExecuted,
		// When no query is executed
		ENotPrepared,
		// When no query is prepared
		ENoResultBound,
		// No results left

		// Of MySQL
		ENoData = 100,
		//
		EDataTruncated = 101,
		// Buffer length too small
	};

	enum EPreparedBind : unsigned char
	{
		PBParam,
		PBResult,
		PBMax,
	};


	struct SConnectionData
	{
		std::string sHost;
		std::string sUsername;
		std::string sPassword;
		std::string sDatabase;

		unsigned short wPort;
		bool fAutoReconnect;

		SConnectionData()
			: wPort(0), fAutoReconnect(false)
		{
		}
	};
}

// Json
namespace Json
{
	enum EJsonType
	{
		JTObject,
		JTNumber,
		JTBoolean,
		JTString,
		JTArray,
		JTNull,
	};
}