#include "stdafx.h"
#include "session.hpp"
#include "console.hpp"
#include <iostream>
#include "Settings.h"

kms::session_t session;

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

	int port = 0;

	try {
		port = std::stoi(argv[2]);
	} catch(std::invalid_argument&) {
		std::cerr << "port was malformed. client is exiting.\n";
		exit(-1);
	} catch(std::out_of_range&) {
		std::cerr << "port was malformed. client is exiting.\n";
		exit(-1);
	}

	session.connect(argv[1], port);
	//session.connect("127.0.0.1", 4000);
	//session.connect("avatar.outland.org", 3000);
	//session.connect("achaea.com", 23);
	//session.connect("1984.ws", 23);
	//session.connect("lord.stabs.org", 23);
	
	session.play();

	return 0;
}

