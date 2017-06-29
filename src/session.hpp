#pragma once
#include "socket.hpp"
#include "telnet.hpp"
#include "thread.hpp"
#include "concurrent_queue.hpp"
#include "console.hpp"
#include "commands.hpp"

#include <iostream>


namespace kms {
	//wrap those memory resources
	template<class T>
	class unique_ptr_t {
		T	m_handle;
	public:
		unique_ptr_t()
			: m_handle((T)0)
		{

		}

		unique_ptr_t(T new_handle) 
			: m_handle(new_handle)
		{

		}
		
		unique_ptr_t& operator=(T new_handle) 
		{
			if (m_handle) {
				throw std::exception("already assigned");
			}
			m_handle = new_handle;
			return *this;
		}

		~unique_ptr_t()
		{
			if (m_handle) {
				delete m_handle;
				m_handle = (T)0;
			}
		}
	};

	class session_t {
		kms::socket_t					m_socket;
		kms::telnet_t					m_telnet;
		unique_ptr_t<thread_base*>		m_readThread;
		unique_ptr_t<thread_base*>		m_writeThread;
		concurrent_queue<std::string>	m_bufferedWrite;
		console_t						m_console;
		commands_t						m_commands;
		
		static void readMud(session_t& session) {
			CCharVector data(4096, 0);
			int index = 0;
			for(;;) {
				::Sleep(30);
				if (session.m_socket.checkForRead()) {
					int rc = session.m_socket.recv(data, index);
					if (rc > 0) {
						index += rc;
					} else if (rc == 0) {
						//connection closed??
					} else {
						printf("\r\n%s\r\n", "Connection Lost");
						break;
					}
				
				//} else {
					if (index > 0) {
						if (session.m_telnet.process(data, index, session.m_console, session.m_bufferedWrite)) {
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
					unsigned char nTest = (unsigned char)sLine[0];
					if (nTest == 255) {
					} else if (nTest == '#') {
						session.m_commands.process(sLine);
						continue;
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
		}

		bool connect(const std::string& sAddress, int nPort)
		{
			return m_socket.connectTcp(sAddress, nPort);
		}

		void play()
		{
			//m_socket.setNonBlockingMode();

			m_readThread = make_thread(&readMud, static_cast<session_t&>(*this));//  new recv_thread_t(&readMud, *this);
			m_writeThread = make_thread(&writeMud, static_cast<session_t&>(*this));// new recv_thread_t(&writeMud, *this);
			/*::Sleep(500);
			m_bufferedWrite.push("mazaliasix");
			m_bufferedWrite.push("y");
			m_bufferedWrite.push("poopoo");
			m_bufferedWrite.push("poopoo");
			::Sleep(2000);
			m_bufferedWrite.push("\r");
			::Sleep(2000);
			m_bufferedWrite.push("human");
			::Sleep(2000);
			m_bufferedWrite.push("Y");
			::Sleep(2000);
			m_bufferedWrite.push("M");
			::Sleep(2000);
			m_bufferedWrite.push("\r");
			::Sleep(2000);
			m_bufferedWrite.push("cle");
			::Sleep(2000);
			m_bufferedWrite.push("Y");
			::Sleep(2000);
			m_bufferedWrite.push("Y");*/

			std::string sInput;
			while(1) {	
				std::getline(std::cin, sInput);
				m_bufferedWrite.push(sInput);
			}
		}
	private:


	
	};


}