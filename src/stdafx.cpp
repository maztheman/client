// stdafx.cpp : source file that includes just the standard includes
// client.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

//platform header must implement this class for init
kms::socket_init kms::socket_init::m_instance;
