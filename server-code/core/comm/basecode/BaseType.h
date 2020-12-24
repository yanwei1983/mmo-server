/** \file BaseType.h
 *
 *	filename: 	BaseType.h
 *	purpose:	Definition
 *	created:	2012-3-20  20:41
 *	author:		zlong
 *	purpose:
 *
 *	(c)2006 - 2010 All Rights Reserved
 ****************************************************************************/

#ifndef __BASETYPE_H__
#define __BASETYPE_H__

#define NOMINMAX
/////////////////////////////////////////////////////////////////////////////////
//当前包含的系统头文件引用
/////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>   
#include <stdint.h>  
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <exception>
#include <string>

#ifdef __linux__
#include <execinfo.h>
#include <pthread.h>
#include <semaphore.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>

using HMODULE = void*;
using HANDLE  = void*;


#ifndef SOCKET
using SOCKET = uint32_t;
#endif

constexpr uint32_t INFINITE = 0xFFFFFFFF; // Infinite timeout

constexpr SOCKET INVALID_SOCKET = static_cast<SOCKET>(-1);
constexpr SOCKET SOCKET_ERROR   = static_cast<SOCKET>(-1);

#endif

using float32_t = float;
using float64_t = double;

using byte = unsigned char;
using BYTE = uint8_t;

using OBJID = uint64_t;



///////////////////////////////////////////////////////////////////////
const OBJID ID_NONE = static_cast<OBJID>(0);

///////////////////////////////////////////////////////////////////////
#define TRUE  1
#define FALSE 0




#define LINEEND "\n"

#define __max(a, b) (((a) > (b)) ? (a) : (b))
#define __min(a, b) (((a) < (b)) ? (a) : (b))

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 256
#include "magic_enum.h"

#define NAMEOF_ENUM_RANGE_MIN 0
#define NAMEOF_ENUM_RANGE_MAX 256
#include "nameof.h"

#endif //__BASETYPE_H__
