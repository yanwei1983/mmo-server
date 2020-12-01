--技能 xxx 101

--注册回调,需要的才注册，不注册不会回调
local this_skill = register_script(SCRIPT_SKILL, 101);



function this_skill.OnDoIntone( pOwner, idTarget, posTarget, pCurSkillType)
    --吟唱时额外XXX
end

function this_skill.OnDoLaunch( pOwner, idTarget, posTarget, pCurSkillType)
    --施法开始时额外XXX
end

function this_skill.IsTarget(pOwner, posTarget, pSkillType, pActor)
    --特殊的伤害判断
    return true; 
end

function this_skill.OnSkillEffect(pOwner, idTarget, posTarget, pSkillType, nApplyTimes)
    --对目标释放完1轮效果
end

function this_skill.OnDoDamage( pOwner, idTarget, posTarget, pSkillType, pTarget)
    --准备进行伤害
end

