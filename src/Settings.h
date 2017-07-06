#pragma once
#include <string>

class Settings
{

	Settings();
	~Settings();
public:

	static std::string Dir;
	static std::string IniFile;

	static void ReadInit();

};

