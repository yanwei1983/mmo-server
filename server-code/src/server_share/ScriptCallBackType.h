#ifndef SCRIPTCALLBACKTYPE_H
#define SCRIPTCALLBACKTYPE_H

#include "magic_enum.h"

export_lua enum ScriptType
{
    SCRIPT_ITEM,
    SCRIPT_MAP,
    SCRIPT_MONSTER,
    SCRIPT_NPC,
    SCRIPT_STATUS,
    SCRIPT_TASK,
    SCRIPT_SKILL,
    SCRIPT_COMMON,
    SCRIPT_AI,
    SCRIPT_MAX
};


#endif /* SCRIPTCALLBACKTYPE_H */
