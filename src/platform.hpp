#pragma once
//this header is clearly for windows
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment (lib, "WS2_32.lib")

namespace kms {
	//REQUIRED: socket init singleton
	class socket_init
	{
		WSADATA m_wsaData;
		int		m_iResult;

		static socket_init m_instance;

		socket_init()
			: m_iResult(-1)
		{
			m_iResult = WSAStartup(MAKEWORD(2,2), &m_wsaData);
		}
		
		~socket_init() {
			WSACleanup();//might clean up too fast
		}

	public:

		static const socket_init& GetInstance() {
			return m_instance;
		}

		bool GetIsValid() const {
			return m_iResult == 0;
		}

	};
}

//REQUIRED: Initialize it.
inline bool SocketInit() { return kms::socket_init::GetInstance().GetIsValid(); }

typedef SOCKET					kms_socket_t;
typedef HANDLE					native_handle_type;

#define KMS_INVALID_SOCKET		INVALID_SOCKET
#define KMS_SOCKET_ERROR		SOCKET_ERROR