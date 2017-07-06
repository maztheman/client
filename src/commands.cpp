#include "stdafx.h"
#include "commands.hpp"
#include <sstream>

static std::vector<std::string> to_vector(std::string s, std::string token)
{
	std::vector<std::string> retval;
	retval.reserve(128);
	auto idx = 0UL;
	while (1) {
		auto found = s.find(token, idx);
		if (found == std::string::npos) {
			if (idx < s.size()) {
				retval.emplace_back(s.substr(idx));
			}
			break;
		}
		auto result = s.substr(idx, found - idx);
		if (result.empty() == false) {
			retval.emplace_back(result);
		}
		idx = found + token.size();
	}
	return retval;
}

void kms::commands_t::OnIncoming(std::string sIncoming)
{
	auto lines = to_vector(sIncoming, "\n\r");
	for (auto& line : lines) {
		for (auto& script : m_IncomingScripts) {
			if (script->OnIncoming(line)) {
				break;
			}
		}
	}
}
