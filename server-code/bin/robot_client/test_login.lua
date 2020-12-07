robot_manager:RegisterCMD(CMD_SC_LOGIN, "OnRecv_SC_LOGIN");
robot_manager:RegisterCMD(CMD_SC_ACTORINFOLIST, "OnRecv_SC_ACTORINFOLIST");

function OnConnect(client)
	print_debugmsg(client, "OnConnect.");
	
	local info = g_clientinfo[client:GetClientID()];
	local msg = ProtobufMessageWarp("CS_LOGIN");
	--msg.openid = info.openid;
	--msg.auth = 
	local server_passwd="test";
	--print_clientmsg(client, "openid=", msg.openid);
	--msg.last_succ_key = md5(msg.openid .. math.floor(TimeGetSecond()/180) .. server_passwd);
	--msg.prog_ver = "0.1.0"
	
	
	msg.openid= string.format("test_%d", client:GetClientID());
	msg.auth = ""
	msg.last_succ_key = "gmtest"
	msg.prog_ver = "0.0.1"
	client:SendProtobufToServer(GetProtobufMessagePtr(msg) );
end

function OnDisconnected(client)
	print_debugmsg(client, "OnDisconnected.");
end

function OnRecv_SC_LOGIN(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_LOGIN");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_LOGIN ParseFromArray fail");
		return;
	end
	
	if(msg.result_code == 0) then
		print_debugmsg(client, "SC_LOGIN succ.");
	else
		print_debugmsg(client,"SC_LOGIN error: ",msg.result_code, " detail: ", msg.detail);
	end
		
end

function OnRecv_SC_ACTORINFOLIST(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_ACTORINFOLIST");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_ACTORINFOLIST ParseFromArray fail");
		return;
	end
	
	local actor_size = GetProtobufFieldSize(msg, "list");
	print_debugmsg(client, "actorlist:", actor_size);
	client:Interrupt();
end




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
