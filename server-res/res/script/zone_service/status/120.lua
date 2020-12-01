--状态 xxx 120

--注册回调,需要的才注册，不注册不会回调
local this_status = register_script(SCRIPT_STATUS, 120);

--当挂接本状态时
function this_status.OnAttach(pStatus)

end

--当结束挂接时
function this_status.OnDetach(pStatus)

end

--当时间到需要作用时
function this_status.OnEffect(pStatus)

end

--当自己攻击别人时
function this_status.OnAttack(pStatus, pTarget)
	
end

--当自己被别人攻击时
function this_status.OnBeAttack(pStatus, pAttacker)

end

--当自己移动时
function this_status.OnMove(pStatus)

end

--当自己登陆时
function this_status.OnLogin(pStatus)

end

--当自己退出登陆时
function this_status.OnLogout(pStatus)

end