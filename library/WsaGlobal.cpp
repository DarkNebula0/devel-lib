#include "WsaGlobal.h"
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


void InitializeWSA() {
	static bool		s_bIsWsaInitialized = false;
	static WSAData	s_oWsaData;

	if (s_bIsWsaInitialized) return;

	WSAStartup(MAKEWORD(2, 2), &s_oWsaData);
	s_bIsWsaInitialized = true;
}