#pragma once
#include <memory>

typedef long long			int64;
typedef unsigned long long	uint64;
typedef unsigned long		DWORD;
typedef unsigned int		uint;
typedef unsigned short		ushort;
typedef unsigned char		byte;
typedef void				*HANDLE;
typedef void				*Function;

struct sockaddr_in;

struct st_mysql;
struct st_mysql_res;
struct st_mysql_field;
struct st_mysql_stmt;
struct st_mysql_bind;

struct HINSTANCE__;
typedef HINSTANCE__ *HMODULE;

class CDir;
class CFile;
class CIniFile;
class CReadStream;
class CWriteStream;

// Network 
namespace Network
{
	typedef size_t SOCKET;
	typedef sockaddr_in SPeer;
	enum EError : byte;

	struct SEndpoint;
	class CPacket;

	namespace TCP {
		enum ETask : unsigned char;

		struct STask;
		struct SAcceptTask;
		struct SConnectTask;
		struct SRecvTask;
		struct SSendTask;

		class CSocket;
		class CClient;
		class CServer;
	}
}

typedef std::shared_ptr<Network::TCP::CSocket>	TCPSocket;
typedef std::weak_ptr<Network::TCP::CSocket>	WeakTCPSocket;

typedef Network::TCP::CSocket CTCPSocket;
typedef Network::TCP::CServer CTCPServer;
typedef Network::TCP::CClient CTCPClient;


// MySQL
namespace MySQL {
	typedef st_mysql		SDatabase;
	typedef st_mysql_field	SField;
	typedef st_mysql_res	SResult;
	typedef st_mysql_stmt	SPreparedStatement;
	typedef st_mysql_bind	SPreparedBind;
	typedef	char			*SRow;

	enum EError : byte;
	enum EPreparedBind : byte;

	class CDatabase;
	class CPreparedQuery;
	class CQuery;
}

typedef MySQL::CDatabase CMySQLDatabase;
typedef MySQL::CQuery CMySQLQuery;
typedef MySQL::CPreparedQuery CMySQLPreparedQuery;

// Plugin
namespace Plugin
{
	class CPlugin;
	class CManager;
}

// Json
namespace Json
{
	class CDocument;
	class CObject;
	class CArray;
}