#pragma once
#include "concurrent_queue.hpp"
#include <regex>

namespace kms {
	class commands_t 
	{
		concurrent_queue<std::string>&	m_bufferedWrite;
	public:
		commands_t(concurrent_queue<std::string>& bufferedWrite)
			: m_bufferedWrite(bufferedWrite)
		{
		}

		std::string process(const std::string& sInput)
		{
			std::string cmd;
			std::stringstream ss(sInput);
			ss >> cmd;

			std::tr1::regex rx("(?<alias>{[a-zA-Z][a-zA-Z0-9 %]}");
			//std::tr1::match_results results;
			//std::tr1::regex_match(sInput.begin(), sInput.end(), results, 

			int look = 0;
			return "";
		}


	};



}