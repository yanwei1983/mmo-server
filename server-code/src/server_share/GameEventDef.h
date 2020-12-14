#ifndef GAMEEVENTDEF_H
#define GAMEEVENTDEF_H

enum GameEventType
{
    EVENTID_UNKNOW = 0,
    EVENTID_DEL_ACTOR,
    EVENTID_FLY_MAP,
    EVENTID_BROCAST_SHOW,
    EVENTID_SEND_ATTRIB_CHANGE,
    EVENTID_PLAYER_ON_TIMER,
    EVENTID_BULLET_APPLY,
    EVENTID_BULLET_MOVESTEP,
    EVENTID_PHASE_TIMECALL,
    EVENTID_PHASE_DEL,
    EVENTID_SKILL_DO_LAUNCH,
    EVENTID_SKILL_DO_APPLY,
    EVENTID_SKILL_DO_IDLE,
    EVENTID_STATUS_CALLBACL,
    EVENTID_MONSTER_GENERATOR,
    EVENTID_MONSTER_AI,
    EVENTID_MONSTER_AI_SEARCHENEMY,
};
#endif /* GAMEEVENTDEF_H */
