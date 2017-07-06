#include "stdafx.h"
#include "session.hpp"
#include "console.hpp"
#include <iostream>
#include "Settings.h"

kms::session_t session;

std::string sPitchBlack = R"gxx(
function incoming(text)
	if text:match("^It is pitch black %.%.%.") then
		jmc.send("wear lantern")
		return 1;
	end

	return 0;
end

)gxx";

std::string sMobFlee1 = R"gxx(
function incoming(text)
	local follow_on = jmc.getVariable("c_follow_on") == "1"
	local c_mob = jmc.getVariable("c_mob")
	local mob,dir = text:match("The ([%a- ]+) has fled([%a- ]*)!")

	if mob == nil and dir == nil then
		mob,dir = text:match("([%a- ]+) has fled([%a- ]*)!")
	end
	
	if dir == nil then
		return 0;	
	end

	if not dir or dir == '' then
		dir = jmc.getVariable("c_dir")
	end

	if follow_on and dir and mob then
		jmc.send(dir)
		jmc.send("kill "..c_mob)
		return 1
	end

	return 0
end

)gxx";

std::string sMobFlee2 = R"gxx(
function incoming(text) 
	local mob,dir = text:match("The ([%a- ]+) flees (%a+) in terror!")
	if not mob and not dir then
		mob,dir = text:match("([%a- ]+) flees ([%a]+) in terror!")
	end
	local follow_on = jmc.getVariable("c_follow_on") == "1"
	local c_mob = jmc.getVariable("c_mob")
	if mob and dir and follow_on and c_mob then
		io.write(string.format("mob: %s going to %s\n", c_mob, dir))		
		jmc.send(dir)
		jmc.send("kill "..c_mob)
		return 1
	end

	return 0
end
)gxx";

std::string sMobFlee3 = R"gxx(
function incoming(text) 
	local mob,verb,dir = text:match("The ([%a- ]+) (%a+) ([%a]+)%.")
	local follow_on = jmc.getVariable("c_follow_on") == "1"
	local c_mob = jmc.getVariable("c_mob")
	if verb ~= nil and (verb == 'leaves' or verb == 'flies') then
		if dir ~= nil and (dir == 'north' or dir == 'south' or dir == 'east' or dir == 'west' or dir == 'up' or dir == 'down') then
			if (mob and dir and c_mob) then
				if follow_on and mob:find(c_mob) then
					jmc.setVariable("c_dir", dir)
	  				return 1
				end
			end
		end
	end
	return 0;
end
)gxx";

std::string sYouAre = R"gxx(
function incoming(text) 
	local verb = text:match("You are (%a+)%.")
	if verb ~= nil then
		if verb == 'Hungry' or verb == 'hungry' then
			jmc.send("eat bread")
			return 1;
		elseif verb == 'Thirsty' or verb == 'thirsty' then
			jmc.send("drink flask")
			return 1;
		end
	end
	return 0
end
)gxx";

std::string sIsDead = R"gxx(
function incoming(text) 
	local mob = text:match("([%a- ]+) is DEAD!!")
	if mob ~= nil then
		jmc.setVariable("c_follow_on", "0")
		return 1
	end
	return 0
end
)gxx";


std::string sAlias = R"gxx(
function alias(input)
	local mob = input:match("^k (%a+)$") 
	if mob ~= nil then
		jmc.setVariable("c_mob", mob)
		jmc.setVariable("c_follow_on", "1")
		jmc.send("kill "..mob)
		return 1;
	end
	return 0;
end
)gxx";





int main(int argc, char* argv[])
{
	std::string sExecutable = argv[0];
	Settings::IniFile = sExecutable + ".ini";
	

	auto idx = sExecutable.find_last_of("/\\");
	Settings::Dir = sExecutable.substr(0, idx + 1);


	if (!SocketInit()) return -1;

	Settings::ReadInit();


	if (argc != 3) {
		std::cout << "client - telnet client made by maztheman 2000 - 2017" << std::endl;
		std::cout << "client.exe <server> <port>" << std::endl;
		exit(1);
	}

	int port = std::strtol(argv[2], NULL, 10);

	session.connect(argv[1], port);
	//session.connect("127.0.0.1", 4000);
	//session.connect("avatar.outland.org", 3000);
	//session.connect("achaea.com", 23);
	//session.connect("1984.ws", 23);
	//session.connect("lord.stabs.org", 23);
	
	session.play();

	return 0;
}

