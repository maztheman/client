#pragma once
#include "utility.hpp"

#include "concurrentqueue.h"
#include "session.hpp"
#include "protected_vector.h"


#include <shared_mutex>
#include <thread>

struct GLFWwindow;

namespace kms {

class console_t
{
public:
	using ro_strings = base_protected_vector_t<std::string>::READ_ONLY_VECTOR;

	console_t();
	~console_t();

	
	void run();
	void writeText(std::string sText);

private:

	

	GLFWwindow* m_window;
	std::mutex m_serverwait;
	base_protected_vector_t<std::string> m_texts;
	protected_vector_t<session_t> m_ConsoleSessions;
	std::vector<std::thread> m_SessionThreads;
};

}