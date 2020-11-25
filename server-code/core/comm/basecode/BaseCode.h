#ifndef BaseCode_h__
#define BaseCode_h__

#include <chrono>
#include <cstdio>
#include <cstring>
#include <deque>
#include <exception>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>

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
