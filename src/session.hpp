#pragma once
#include "socket.hpp"
#include "telnet.hpp"
#include "concurrent_queue.hpp"
#include "console.hpp"
#include "commands.hpp"
#include "Settings.h"

#include <iostream>
#include <thread>
#include <regex>

namespace kms {

	class session_t {
		typedef std::vector<lua_script_t*> lua_script_v;


		kms::socket_t					m_socket;
		kms::telnet_t					m_telnet;
		concurrent_queue<std::string>	m_bufferedWrite;
		console_t						m_console;
		commands_t						m_commands;
		std::thread						m_recvThread;
		std::thread						m_sendThread;
		lua_script_v					m_aliases;
		
		static void readMud(session_t& session) {
			CCharVector data(4096, 0);
			size_t index = 0;
			for(;;) {
				std::this_thread::sleep_for(std::chrono::milliseconds(30));
				if (session.m_socket.checkForRead()) {
					auto rc = session.m_socket.recv(data, index);
					if (rc > 0) {
						index += static_cast<size_t>(rc);
					} else if (rc == 0) {
						//connection closed??
					} else {
						printf("\r\n%s\r\n", "Connection Lost");
						break;
					}
				
				//} else {
					if (index > 0) {
						if (session.m_telnet.process(data, index, session.m_console, session.m_commands, session.m_bufferedWrite)) {
							index = 0;
							data.resize(4096, 0);
						}
					}
				}
				//}
			}
			session.m_socket.close();
		}

		static void writeMud(session_t& session) {
			for(;;) {
				if (session.m_socket.checkForWrite() == 0)	{ continue; }
				if (!session.m_socket.getIsValid())			{ break; }

				//dont care about CoW...
				std::string sLine;
				if (session.m_bufferedWrite.try_pop(&sLine)) {
					//ugly.
					unsigned char nTest = static_cast<unsigned char>(sLine[0]);
					if (nTest == 255) {
					} else {
						sLine += "\r\n";
					}
					session.m_socket.send(sLine);
				}
			}
		}

	public:
		session_t()
			: m_commands(m_bufferedWrite)
		{

		}

		~session_t() 
		{
			ResetAlias();
		}

		bool connect(const std::string& sAddress, int nPort)
		{
			return m_socket.connectTcp(sAddress, nPort);
		}

		void SendToServer(std::string sText)
		{
			m_bufferedWrite.push(sText);
		}

		void play()
		{
			std::thread r(readMud, std::ref(*this));
			std::thread w(writeMud, std::ref(*this));

			m_recvThread.swap(r);
			m_sendThread.swap(w);

			std::string sInput;
			while(1) {	
				std::getline(std::cin, sInput);
				if (sInput == "exit") {
					m_bufferedWrite.push("quit");
					break;
				} else if (sInput == "/reload") {
					Settings::ReadInit();
					continue;
				}
				bool rc = false;
				//this is where alias should be processed man...
				for (auto& alias : m_aliases) {
					if (alias->OnIncoming(sInput)) {
						rc = true;
						break;
					}
				}

				if (rc == false) {
					m_bufferedWrite.push(sInput);
				}
			}
		}

		void AddAlias(const std::string& sCode, std::string sFunction)
		{
			m_aliases.push_back(new lua_script_t(sCode, sFunction));
		}

		void ResetAlias()
		{
			for (auto& p : m_aliases) {
				delete p;
			}
			m_aliases.clear();
		}

		commands_t& Commands() {
			return m_commands;
		}
	private:


	
	};


}

inline kms::session_t& Session() {
	extern kms::session_t session;
	return session;
}