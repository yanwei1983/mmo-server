robot_manager:RegisterCMD(CMD_SC_LOGIN, "OnRecv_SC_LOGIN");
robot_manager:RegisterCMD(CMD_SC_ACTORINFOLIST, "OnRecv_SC_ACTORINFOLIST");
robot_manager:RegisterCMD(CMD_SC_CREATEACTOR, "OnRecv_SC_CREATEACTOR");
robot_manager:RegisterCMD(CMD_SC_LOADMAP, "OnRecv_SC_LOADMAP");
robot_manager:RegisterCMD(CMD_SC_ENTERMAP, "OnRecv_SC_ENTERMAP");
robot_manager:RegisterCMD(CMD_SC_PLAYERINFO, "OnRecv_SC_PLAYERINFO");
robot_manager:RegisterCMD(CMD_SC_AOI_UPDATE, "OnRecv_SC_AOI_UPDATE");
robot_manager:RegisterCMD(CMD_SC_ATTRIB_CHANGE, "OnRecv_SC_ATTRIB_CHANGE");
robot_manager:RegisterCMD(CMD_SC_DEAD, "OnRecv_SC_DEAD");
robot_manager:RegisterCMD(CMD_SC_DAMAGE, "OnRecv_SC_DAMAGE");

--robot_manager:RegisterCMD(CMD_SC_AOI_NEW, "OnRecv_SC_AOI_NEW");
--robot_manager:RegisterCMD(CMD_SC_AOI_REMOVE, "OnRecv_SC_AOI_REMOVE");


function OnConnect(client)
	if g_print_debug then
		print_clientmsg(client, "OnConnect.");
	end
	local info = g_clientinfo[client:GetClientID()];
	local msg = ProtobufMessageWarp("CS_LOGIN");
	--msg.openid = info.openid;
	--msg.auth = 
	local server_passwd="test";
	--print_clientmsg(client, "openid=", msg.openid);
	--msg.last_succ_key = md5(msg.openid .. math.floor(TimeGetSecond()/180) .. server_passwd);
	--msg.prog_ver = "0.1.0"
	
	
	msg.openid="12345"
	msg.auth = ""
	msg.last_succ_key = "gmtest"
	msg.prog_ver = "0.0.1"
	client:SendProtobufToServer(GetProtobufMessagePtr(msg) );
end

function OnDisconnected(client)
	if g_print_debug then
			print_clientmsg(client, "OnDisconnected.");
	end
end

function OnRecv_SC_LOGIN(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_LOGIN");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_LOGIN ParseFromArray fail");
		return;
	end
	
	if(msg.result_code == 0) then
		if g_print_debug then
			print_clientmsg(client, "SC_LOGIN succ.");
		end
	else
		if g_print_debug then
			print_clientmsg(client,"SC_LOGIN error: ",msg.result_code, " detail: ", msg.detail);
		end
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
		if g_print_debug then
			print_clientmsg(client,"send CS_SELECTACTOR");
		end
		local send_msg = ProtobufMessageWarp("CS_SELECTACTOR");
		client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
	else
		if g_print_debug then
			print_clientmsg(client,"SC_CREATEACTOR error:",msg.result_code);
		end
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
		if g_print_debug then
			print_clientmsg(client,"send CS_CREATEACTOR");
		end
		
		local send_msg = ProtobufMessageWarp("CS_CREATEACTOR");
		send_msg.name = "test_"..client:GetClientID();
		send_msg.prof = 1;
		send_msg.baselook = 1;
		client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
	else
		--select actor
		if g_print_debug then
			print_clientmsg(client,"send CS_SELECTACTOR");
		end
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
	
	if g_print_debug then
		print_clientmsg(client, "SC_PLAYERINFO recv. hp:", info.hp);
	end
		
end

function OnRecv_SC_ATTRIB_CHANGE(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_ATTRIB_CHANGE");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_ATTRIB_CHANGE ParseFromArray fail");
		return;
	end
	local info = g_clientinfo[client:GetClientID()];
	if(info.playerid == msg.actor_id)then
		local size_datalist = msg.datalist:size();
		for i=0,size_datalist-1,1 do
			if msg.datalist[i].actype == PROP_HP then
				info.hp = msg.datalist[i].val;
				if g_print_debug then
					print_clientmsg(client, "hp=", info.hp);
				end
			end
		end
	else
	end
	
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

	
	if(info.playerid == msg.actor_id)then
		info.posx=msg.posx;
		info.posy=msg.posy;
		if g_print_debug then
			print_clientmsg(client, "self posx:", info.posx, " posy:", info.posy);
		end
		client:AddEventCallBack(random_uint32_range(700,1500), "SendMove", false);
	else
		--other actor
		if g_print_debug then
			print_clientmsg(client, string.format("%d posx:%f posy:%f", msg.actor_id, info.posx, info.posy));
		end
	end
	
end


function OnRecv_SC_LOADMAP(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_LOADMAP");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_LOADMAP ParseFromArray fail");
		return;
	end
	if g_print_debug then
		print_clientmsg(client, "loading map:", msg.mapid, ",x:", msg.posx, ",y:", msg.posy);
	end
	
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
	if g_print_debug then
		print_clientmsg(client, "enter map succ.");
	end
	local info = g_clientinfo[client:GetClientID()];
	if info.hp == 0 then
		local send_msg = ProtobufMessageWarp("CS_REBORN");
		send_msg.reborn_type = 2;
		client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
		if g_print_debug then
			print_clientmsg(client, "send reborn");
		end
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
		if g_print_debug then
			print_clientmsg(client, "player dead.");
		end
		local send_msg = ProtobufMessageWarp("CS_REBORN");
		send_msg.reborn_type = 2;
		client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
		if g_print_debug then
			print_clientmsg(client, "send reborn");
		end
	end
end


function OnRecv_SC_DAMAGE(client, buffer, size)
	local msg = ProtobufMessageWarp("SC_DAMAGE");
	if(GetProtobufMessagePtr(msg):ParseFromArray(buffer,size) == false) then
		error("SC_DAMAGE ParseFromArray fail");
		return;
	end
	
	if g_print_debug then
		print_clientmsg(client, string.format("attacker %d damage %d", msg.attacker_id, msg.damage) );
	end
end

function SendMove(client)
	
	local send_msg = ProtobufMessageWarp("CS_MOVE");
	local info = g_clientinfo[client:GetClientID()];
	send_msg.scene_idx=info.scene_idx;
	send_msg.x = info.posx + random_float(-1.0, 1.0);
	send_msg.y = info.posy + random_float(-1.0, 1.0);
	client:SendProtobufToServer(GetProtobufMessagePtr(send_msg) );
	if g_print_debug then
		print_clientmsg(client, "sendmove x:", send_msg.x, ",y:", send_msg.y);
	end
	
end
