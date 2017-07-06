#pragma once
#include <string>

struct lua_State;


class lua_script_t
{
	lua_State*	mState;
	std::string mFunction;
public:
	lua_script_t(const std::string& sCode, std::string sFunction);
	~lua_script_t();

	bool OnIncoming(const std::string& text);
};

