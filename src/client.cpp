#include "stdafx.h"
#include "console.hpp"

int main()
{
	//for god sakes use std::filesystem!
	if (!SocketInit()) 
	{
		fmt::print(stderr, "Sockets could not initialize, exiting program.\n");
		return -1;
	}

	//the console is the main program now.
	//it should control everything
	kms::console_t console;
	console.run();
	return 0;
}

