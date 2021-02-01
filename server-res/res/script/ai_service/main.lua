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


function print_table ( t )  
    local print_r_cache={}
    local function sub_print_r(t,indent)
        if (print_r_cache[tostring(t)]) then
            __my_print(indent.."*"..tostring(t))
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        __my_print(indent.."["..pos.."] => "..tostring(t).." {")
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        __my_print(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val)=="string") then
                        __my_print(indent.."["..pos..'] => "'..val..'"')
                    else
                        __my_print(indent.."["..pos.."] => "..tostring(val))
                    end
                end
            else
                __my_print(indent..tostring(t))
            end
        end
    end
    if (type(t)=="table") then
        __my_print(tostring(t).." {")
        sub_print_r(t,"  ")
        __my_print("}")
    else
        sub_print_r(t,"  ")
    end
    __my_print()
end

function main()
    --遍历所有文件
    script_manager:RegistScriptType(SCRIPT_AI, "g_SCRIPT_AI");
	script_manager:LoadFilesInDir("ai", false);
end

