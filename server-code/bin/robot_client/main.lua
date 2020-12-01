

require "robot_client"

InitBaseCodeInLua(this_lua);
g_clientinfo = {} or g_clientinfo;
g_print_debug = false
function print_clientmsg(client,...)
	print(string.format("[%s]", client:GetClientID()), ...)
end
function print_debugmsg(client,...)
	if g_print_debug then
		print(string.format("[%s]", client:GetClientID()), ...)
	end
end

function main(start_idx, max_players)
	
	print("enter main start:",start_idx, " to ", start_idx+max_players-1)
	for i=start_idx,max_players+start_idx-1 do
		local ip = "172.28.1.254";
		local port_list = {18031,18032};
		local port = port_list[random_uint32_range(1,#port_list)];
		print("connect to ", ip, port);
		local pClient = robot_manager:ConnectServer(ip,port);
		if(pClient) then
			pClient:SetClientID(i);
			g_clientinfo[i] ={openid="player_"..i};
		end
	end
	
end
