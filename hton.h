#pragma once
#include "types.h"
// htonl ...
#ifndef _MSC_VER
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#endif

extern "C" uint64_t htonll(uint64_t v);

extern "C" uint64_t ntohll(uint64_t v);
