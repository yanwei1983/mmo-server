--NPC 小鸡 300101001

--注册回调,需要的才注册，不注册不会回调
local this_npc = register_script(SCRIPT_NPC, 1010001);

function this_npc.OnBorn(pNpc)

end

function this_npc.OnActive(pNpc, pPlayer)
    return true; --继续执行cxx代码
end

