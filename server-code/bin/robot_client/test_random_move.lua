robot_manager:RegisterCMD(CMD_SC_LOGIN, "OnRecv_SC_LOGIN");
robot_manager:RegisterCMD(CMD_SC_ACTORINFOLIST, "OnRecv_SC_ACTORINFOLIST");
robot_manager:RegisterCMD(CMD_SC_CREATEACTOR, "OnRecv_SC_CREATEACTOR");
robot_manager:RegisterCMD(CMD_SC_LOADMAP, "OnRecv_SC_LOADMAP");
robot_manager:RegisterCMD(CMD_SC_ENTERMAP, "OnRecv_SC_ENTERMAP");
robot_manager:RegisterCMD(CMD_SC_PLAYERINFO, "OnRecv_SC_PLAYERINFO");
robot_manager:RegisterCMD(CMD_SC_AOI_UPDATE, "OnRecv_SC_AOI_UPDATE");
robot_manager:RegisterCMD(CMD_SC_PROPERTY_CHANGE, "OnRecv_SC_PROPERTY_CHANGE");
robot_manager:RegisterCMD(CMD_SC_DEAD, "OnRecv_SC_DEAD");
robot_manager:RegisterCMD(CMD_SC_DAMAGE, "OnRecv_SC_DAMAGE");
robot_manager:RegisterCMD(CMD_SC_POS_CHANGE, "OnRecv_SC_POS_CHANGE");

--robot_manager:RegisterCMD(CMD_SC_AOI_NEW, "OnRecv_SC_AOI_NEW");
--robot_manager:RegisterCMD(CMD_SC_AOI_REMOVE, "OnRecv_SC_AOI_REMOVE");


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

function OnRecv_SC_CREATEACTOR(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_CREATEACTOR");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_CREATEACTOR ParseFromArray fail");
		return;
	end
	if msg.result_code == 0 then
		--select actor
		print_debugmsg(client,"send CS_SELECTACTOR");
		local send_msg = ProtobufMessageWarp("CS_SELECTACTOR");
		client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
	else
		print_debugmsg(client,"SC_CREATEACTOR error:",msg.result_code);
	end
end

function OnRecv_SC_ACTORINFOLIST(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_ACTORINFOLIST");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_ACTORINFOLIST ParseFromArray fail");
		return;
	end
	
	local actor_size = GetProtobufFieldSize(msg, "list");
	if(actor_size == 0)then
		--create actor
		print_debugmsg(client,"send CS_CREATEACTOR");
		
		local send_msg = ProtobufMessageWarp("CS_CREATEACTOR");
		send_msg.name = "test_"..client:GetClientID();
		send_msg.prof = 1;
		send_msg.baselook = 1;
		client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
	else
		--select actor
		print_debugmsg(client,"send CS_SELECTACTOR");

		local send_msg = ProtobufMessageWarp("CS_SELECTACTOR");
		client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
		
		
	end
end

function OnRecv_SC_PLAYERINFO(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_PLAYERINFO");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_PLAYERINFO ParseFromArray fail");
		return;
	end
	
	local info = g_clientinfo[client:GetClientID()];
	info.name = msg.name;
	info.playerid = msg.playerid;
	info.hp = msg.hp;
	
	print_debugmsg(client, "SC_PLAYERINFO recv. hp:", info.hp);
		
end

function OnRecv_SC_PROPERTY_CHANGE(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_PROPERTY_CHANGE");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_PROPERTY_CHANGE ParseFromArray fail");
		return;
	end
	local info = g_clientinfo[client:GetClientID()];
	if(info.playerid ~= msg.actor_id)then
		return
	end
	
	local size_datalist = msg.datalist:size();
	for i=0,size_datalist-1,1 do
		if msg.datalist[i].actype == PROP_HP then
			info.hp = msg.datalist[i].val;
			print_debugmsg(client, "hp=", info.hp);
		end
	end
	
	
end

function OnRecv_SC_POS_CHANGE(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_POS_CHANGE");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_POS_CHANGE ParseFromArray fail");
		return;
	end
	
	local info = g_clientinfo[client:GetClientID()];
	if info.scene_idx ~= msg.scene_idx then
		return;
	end

	info.posx=msg.posx;
	info.posy=msg.posy;
	print_debugmsg(client, "self posx:", info.posx, " posy:", info.posy);

	client:AddEventCallBack(random_uint32_range(700,1500), "SendMove", false);
end

function OnRecv_SC_AOI_UPDATE(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_AOI_UPDATE");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_AOI_UPDATE ParseFromArray fail");
		return;
	end
	
	local info = g_clientinfo[client:GetClientID()];
	if info.scene_idx ~= msg.scene_idx then
		return;
	end

	--other actor
	print_debugmsg(client, string.format("%d posx:%f posy:%f", msg.actor_id, info.posx, info.posy));
	
	
end


function OnRecv_SC_LOADMAP(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_LOADMAP");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_LOADMAP ParseFromArray fail");
		return;
	end
	print_debugmsg(client, "loading map:", msg.mapid, ",x:", msg.posx, ",y:", msg.posy);
	
	local info = g_clientinfo[client:GetClientID()];
	info.scene_idx= msg.scene_idx;
	info.posx= msg.posx;
	info.posy= msg.posy;
	
	local send_msg = ProtobufMessageWarp("CS_LOADMAP_SUCC");
	client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
end

function OnRecv_SC_ENTERMAP(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_ENTERMAP");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_ENTERMAP ParseFromArray fail");
		return;
	end
	print_debugmsg(client, "enter map succ.");
	if g_succ_clinet[client:GetClientID()] == nil then
		g_succ_clinet[client:GetClientID()] = true;
		g_succ_clinet_count = g_succ_clinet_count+1;
	end
	local info = g_clientinfo[client:GetClientID()];
	info.record_posx = msg.x;
	info.record_posy = msg.y;
	if info.hp == 0 then
		local send_msg = ProtobufMessageWarp("CS_REBORN");
		send_msg.reborn_type = 2;
		client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
		print_debugmsg(client, "send reborn");

	else
		client:AddEventCallBack(random_uint32_range(700,1500), "SendMove", false);
	end
end


function OnRecv_SC_DEAD(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_DEAD");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_DEAD ParseFromArray fail");
		return;
	end
	
	local info = g_clientinfo[client:GetClientID()];
	if(info.playerid == msg.actor_id)then
		print_debugmsg(client, "player dead.");

		local send_msg = ProtobufMessageWarp("CS_REBORN");
		send_msg.reborn_type = 2;
		client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
		print_debugmsg(client, "send reborn");

	end
end


function OnRecv_SC_DAMAGE(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_DAMAGE");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_DAMAGE ParseFromArray fail");
		return;
	end
	
	print_debugmsg(client, string.format("attacker %d damage %d", msg.attacker_id, msg.damage) );

end

function SendMove(client)
	
	local send_msg = ProtobufMessageWarp("CS_MOVE");
	local info = g_clientinfo[client:GetClientID()];
	send_msg.scene_idx=info.scene_idx;
	local target_pos = info.target_pos;
	if(target_pos == nil)then
		info.target_pos = Vector2(info.record_posx, info.record_posy);
		target_pos = info.target_pos;
		print_debugmsg(client, "target_pos x:", target_pos.x, ",y:", target_pos.y);
	end
	local cur_pos = Vector2(info.posx, info.posy);
	local dis = GameMath.distance(target_pos, cur_pos);
	print_debugmsg(client, "dis:", dis);
	if dis < 0.1 then
		if target_pos.x > 10 then
			target_pos.x = info.record_posx + random_float(-10, 10);
		else
			target_pos.x = info.record_posx + random_float(0, 20);
		end
		if target_pos.y > 10 then
			target_pos.y = info.record_posy + random_float(-10, 10);
		else
			target_pos.y = info.record_posy + random_float(0, 20);
		end
		print_debugmsg(client, "target_new_pos x:", target_pos.x, ",y:", target_pos.y);
	end
	local dir = target_pos - cur_pos;
	local new_dis = dir:normalise();
	--print_debugmsg(client, "dir_normalise x:", dir.x, ",y:", dir.y);
	local new_pos = cur_pos + dir * random_float(math.min(new_dis,0.2), 1.5);

	send_msg.x = new_pos.x;
	send_msg.y = new_pos.y;
	
	client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
	print_debugmsg(client, "sendmove x:", send_msg.x, ",y:", send_msg.y);
	
end




InitBaseCodeInLua(this_lua);
g_clientinfo = {} or g_clientinfo;
g_print_debug = true
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
	if(max_players > 1) then
		g_print_debug = false
	end

	for i=start_idx,max_players+start_idx-1 do
		local ip = "172.28.1.254";
		local port_list = {18031};
		local port = port_list[random_uint32_range(1,#port_list)];
		print("connect to ", ip, port);
		local pClient = robot_manager:ConnectServer(ip,port);
		if(pClient) then
			pClient:SetClientID(i);
			g_clientinfo[i] ={openid="player_"..i};
		end
	end
	
	if(g_print_debug == false)then
		robot_manager:AddTimedCallback(2000, "OnTimer", true);
	end
end

g_succ_clinet={}
g_succ_clinet_count = 0;
function OnTimer()
	print("client:", robot_manager:GetClientCount(), "enter:", g_succ_clinet_count);
end