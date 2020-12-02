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
	if table_name == nil then
		error(string.format("%s can not find a table_name", script_type));
		return;
	end
	local script_table = _G[table_name];
	if script_table == nil then
		error(string.format("%s is not find in global", table_name));
		return;
	end
	local script = script_table[id];
	script = script or {};
	script_manager:RegistFucName(script_type, id);
	return script;
end



function main()
    --遍历所有文件
    script_manager:RegistScriptType(SCRIPT_AI, "g_SCRIPT_AI");
	script_manager:LoadFilesInDir("ai", false);
end

