--物品脚本 10100001

--注册回调,需要的才注册，不注册不会回调
local this_item = register_script(SCRIPT_ITEM, 10100001);

function this_item.OnItemUse(pPlayer, pItem, nCount)
	--增加数值
	pPlayer:AddProperty(PROP_HP, 100*nCount);
	return true;
end
