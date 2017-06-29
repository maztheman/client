#include "stdafx.h"
#include "session.hpp"
#include "console.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
	if (!SocketInit()) return -1;

	if (argc != 3) {
		std::cout << "client - telnet client made by maztheman 2000 - 2017" << std::endl;
		std::cout << "client.exe <server> <port>" << std::endl;
		exit(1);
	}

	int port = std::strtol(argv[2], NULL, 10);

	kms::session_t session;
	session.connect(argv[1], port);
	//session.connect("127.0.0.1", 4000);
	//session.connect("avatar.outland.org", 3000);
	//session.connect("achaea.com", 23);
	//session.connect("1984.ws", 23);
	//session.connect("lord.stabs.org", 23);
	
	session.play();

	return 0;
}

