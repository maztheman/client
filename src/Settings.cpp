#include "stdafx.h"
#include "Settings.h"
#include "session.hpp"
#include <fstream>
#include <sstream>

std::string Settings::Dir;
std::string Settings::IniFile;

Settings::Settings()
{
}


Settings::~Settings()
{
}

static std::string ReadToFile(std::string file)
{
	std::ifstream t(file.c_str());
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return str;
}

void Settings::ReadInit()
{
	std::fstream ini(IniFile.c_str(), std::ios::in);
	if (!ini) {
		return;
	}

	Session().ResetAlias();
	Session().Commands().ResetScripts();

	std::string line;
	while (std::getline(ini, line)) {
		std::stringstream ss(line);
		std::string verb,verb_value, file, function;
		std::getline(ss, verb, '=');
		std::getline(ss, verb_value, '=');
		std::stringstream verbSS(verb_value);
		std::getline(verbSS, file, ',');
		std::getline(verbSS, function, ',');
		if (verb == "alias") {
			Session().AddAlias(ReadToFile(Dir + file), function);
		} else if (verb == "incoming") {
			Session().Commands().AddScripts(ReadToFile(Dir + file), function);
		}
	}

}
