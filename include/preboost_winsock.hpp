#pragma once
// #define _WINSOCKAPI_

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>