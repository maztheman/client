#include "stdafx.h"
#include "console.hpp"
#include "Settings.h"

int main()
{
	//for god sakes use std::filesystem!
	if (!SocketInit()) 
	{
		fmt::print(stderr, "Sockets could not initialize, exiting program.\n");
		return -1;
	}

	Settings::Dir = std::filesystem::current_path();
	Settings::IniFile = std::filesystem::current_path() / "config.yaml";

	//the console is the main program now.
	//it should control everything
	kms::console_t console;
	console.run();
	return 0;
}

