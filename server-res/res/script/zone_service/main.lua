function string.split(str, delimiter)
	if str==nil or str=='' or delimiter==nil then
		return nil
	end
	
    local result = {}
    for match in (str..delimiter):gmatch("(.-)"..delimiter) do
        table.insert(result, match)
    end
    return result
end

function stripextension(filename)  
    local idx = filename:match(".+()%.%w+$")  
    if(idx) then  
        return filename:sub(1, idx-1)  
    else  
        return filename  
    end  
end


function register_script(script_type, id)
	local table_name = script_manager:ScriptTypeToName(script_type);
	local script_table = _G[table_name];
	local script = script_table[id];
	script = script or {};
	script_manager:RegistFucName(script_type, id);
	return script;
end

function main()
	--遍历所有文件
	script_manager:RegistScriptType(SCRIPT_MONSTER, "g_SCRIPT_MONSTER");
	script_manager:LoadFilesInDir("monster", false);
	script_manager:RegistScriptType(SCRIPT_MAP, "g_SCRIPT_MAP");
	script_manager:LoadFilesInDir("map", false);
	script_manager:RegistScriptType(SCRIPT_NPC, "g_SCRIPT_NPC");
	script_manager:LoadFilesInDir("npc", false);
	script_manager:RegistScriptType(SCRIPT_TASK, "g_SCRIPT_TASK");
	script_manager:LoadFilesInDir("task", false);
	script_manager:RegistScriptType(SCRIPT_ITEM, "g_SCRIPT_ITEM");
	script_manager:LoadFilesInDir("item", false);
	script_manager:RegistScriptType(SCRIPT_SKILL, "g_SCRIPT_SKILL");
	script_manager:LoadFilesInDir("skill", false);
	script_manager:RegistScriptType(SCRIPT_STATUS, "g_SCRIPT_STATUS");
	script_manager:LoadFilesInDir("status", false);
	script_manager:RegistScriptType(SCRIPT_COMMON, "g_SCRIPT_COMMON");
	script_manager:LoadFilesInDir("system", false);
end

