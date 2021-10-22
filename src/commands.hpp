#pragma once
#include <regex>
#include <map>
#include <thread>
#include "concurrentqueue.h"
#include "lua_script.h"

namespace kms {
	class commands_t 
	{
		using queue_type = moodycamel::ConcurrentQueue<std::string>;
		using new_queue_type = moodycamel::ConcurrentQueue<std::string>;
		using map_type = std::map<std::string, std::string>;
		using lua_script_v = std::vector<lua_script_t*>;

		queue_type&	m_bufferedWrite;
		map_type	m_variables;
		lua_script_v m_IncomingScripts;
		std::thread m_tIncoming;
		new_queue_type	m_bufferedIncoming;

		std::string GetSimpleMob(std::string sMob)
		{
			auto itc = std::find(sMob.begin(), sMob.end(), ' ');
			return (itc != sMob.end()) ? std::string(sMob.begin(), itc) : sMob;
		}

		void OnIncoming(std::string sIncoming);

	public:
		commands_t(queue_type& bufferedWrite)
			: m_bufferedWrite(bufferedWrite)
			, m_bufferedIncoming(100U)
		{
			auto func = [this](commands_t& This, new_queue_type& bufferedIncoming) {
				std::string sText;
				sText.reserve(4096);
				for (;;) {
					if (bufferedIncoming.try_dequeue(sText)) {
						This.OnIncoming(sText);
					}
				}
			};

			std::thread th(func, std::ref(*this), std::ref(m_bufferedIncoming));
			m_tIncoming.swap(th);
		}

		~commands_t()
		{
			ResetScripts();
		}

		map_type& Variables() {
			return m_variables;
		}

		void AddScripts(const std::string& sCode, std::string sFunction)
		{
			m_IncomingScripts.push_back(new lua_script_t(sCode, sFunction));
		}

		void ResetScripts()
		{
			for (auto& p : m_IncomingScripts) {
				delete p;
			}
			m_IncomingScripts.clear();
		}

		void AddIncoming(std::string sText) {
			m_bufferedIncoming.enqueue(sText);
		}
	};
}
