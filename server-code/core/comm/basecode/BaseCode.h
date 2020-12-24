#ifndef BaseCode_h__
#define BaseCode_h__

#include <chrono>
#include <cstdio>
#include <cstring>
#include <stdint.h>       // for uint32_t, int32_t
#include <stddef.h>  

#include <exception>
#include <string>
#include <string_view>

#include <vector>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <utility>
#include <memory>         // for unique_ptr, shared_ptr


#ifdef __linux__
#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#include <windows.h>
#endif


#include "BaseType.h"
#include "CheckUtil.h"
#include "GameMath.h"
#include "IntUtil.h"
#include "LoggingMgr.h"
#include "Misc.h"
#include "Noncopyable.h"
#include "StringAlgo.h"
#include "TimeUtil.h"
#include "export_lua.h"

export_lua enum SYNC_TYPE { SYNC_FALSE, SYNC_TRUE, SYNC_ALL, SYNC_ALL_DELAY };
export_lua constexpr bool UPDATE_TRUE  = true;
export_lua constexpr bool UPDATE_FALSE = false;

#endif // BaseCode_h__
