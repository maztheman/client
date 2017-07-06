#include "stdafx.h"
#include "lua_script.h"
#include "session.hpp"
#include "Settings.h"

#include <lua.hpp>

#pragma comment(lib, "lual.lib")

static const std::string sBaseLibrary = R"gxx(
jmc = {
	send = function(text) return kms_server_send(text) end,
	setVariable = function(vr, val) return kms_set_variable(vr, val) end,
	getVariable = function(vr) return kms_get_variable(vr) end,
}
)gxx";

#ifdef __cplusplus
extern "C" {
#endif
	int kms_server_send(lua_State* state)
	{
		int args = lua_gettop(state);
		for (int n = 1; n <= args; n++) {
			std::string text = lua_tostring(state, n);
			printf("[SCRIPT:Send] ==> %s\n", text.c_str());
			Session().SendToServer(text);
		}
		return 0;
	}

	int kms_set_variable(lua_State* state)
	{
		int args = lua_gettop(state);
		if (args == 2) {
			std::string var = lua_tostring(state, 1);
			std::string val = lua_tostring(state, 2);
			printf("[SCRIPT:SetVariable] %s = %s\n", var.c_str(), val.c_str());
			Session().Commands().Variables()[var] = val;

		}
		return 0;
	}
	int kms_get_variable(lua_State* state)
	{
		int args = lua_gettop(state);
		if (args == 1) {
			std::string var = lua_tostring(state, 1);
			std::string& val = Session().Commands().Variables()[var];
			lua_pushstring(state, val.c_str());
			return 1;
		}
		return 0;
	}
#ifdef __cplusplus
}
#endif

lua_script_t::lua_script_t(const std::string& sCode, std::string sFunction)
	: mState(luaL_newstate())
	, mFunction(sFunction)
{
	luaL_openlibs(mState);
	int retval = luaL_dostring(mState, sBaseLibrary.c_str());
	if (retval != 0) {
		std::string error = lua_tostring(mState, -1);
		int tt = 0;
	}
	lua_register(mState, "kms_server_send", kms_server_send);
	lua_register(mState, "kms_set_variable", kms_set_variable);
	lua_register(mState, "kms_get_variable", kms_get_variable);
	retval = luaL_dostring(mState, sCode.c_str());
	if (retval != 0) {
		std::string error = lua_tostring(mState, -1);
		int tt = 0;
	}
}


lua_script_t::~lua_script_t()
{
	lua_close(mState);
}

bool lua_script_t::OnIncoming(const std::string& text)
{
	lua_getglobal(mState, mFunction.c_str());
	lua_pushstring(mState, text.c_str());
	int result = lua_pcall(mState, 1, LUA_MULTRET, 0);
	if (result != 0) {
		std::string error = lua_tostring(mState, -1);
		int tt = 0;
	}
	int retval = lua_tonumber(mState, -1);
	lua_pop(mState, 1);
	return retval != 0;
}
