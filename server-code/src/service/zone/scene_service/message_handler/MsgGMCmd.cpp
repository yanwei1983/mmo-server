#include "GMManager.h"
#include "Player.h"

void OnGMCMD_help(CPlayer* pPlayer, const std::vector<std::string>& vecCMD)
{
    __ENTER_FUNCTION
    __LEAVE_FUNCTION
}

void OnGMCMD_additem(CPlayer* pPlayer, const std::vector<std::string>& vecCMD)
{
    __ENTER_FUNCTION
    CHECK(pPlayer);
    if(vecCMD.size() < 3)
        return;

    uint32_t idItemType = std::stoul(vecCMD[1]);
    uint32_t nCount     = std::stoul(vecCMD[2]);
    uint32_t dwFlag     = 0;
    if(vecCMD.size() >= 4)
        dwFlag = std::stoul(vecCMD[3]);
    pPlayer->AwardItem(0, idItemType, nCount, dwFlag);
    __LEAVE_FUNCTION
}

void OnGMCMD_chgmap(CPlayer* pPlayer, const std::vector<std::string>& vecCMD)
{
    __ENTER_FUNCTION
    CHECK(pPlayer);
    if(vecCMD.size() < 3)
        return;

    uint32_t idMapID    = std::stoul(vecCMD[1]);
    uint32_t posx       = std::stoul(vecCMD[2]);
    uint32_t posy       = std::stoul(vecCMD[3]);
    uint16_t idPhase    = std::stol(vecCMD[4]);
    uint16_t nPhaseType = std::stol(vecCMD[5]);

    pPlayer->FlyMap(idMapID, idPhase, nPhaseType, posx, posy, 0, 0);
    __LEAVE_FUNCTION
}

void CGMManager::GMCmdHandlerRegister()
{
#define REG_GMCMD(name) RegisterGMCmd(#name, &OnGMCMD_##name);
    REG_GMCMD(help);
    REG_GMCMD(additem);
    REG_GMCMD(chgmap);
#undef REG_GMCMD
}