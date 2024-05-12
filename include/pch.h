#pragma once

// Добавьте сюда заголовочные файлы для предварительной компиляции

#define OPCUACLENT
#define MAX_STRING 1024

#include <strsafe.h>
#include <shlobj.h>
#include <stdlib.h>
#include <direct.h>
#include <wtsapi32.h>
#include <psapi.h>
#include <userenv.h>
#include <chrono>
#include <queue>
//#include <mutex>

#include <gdiplus.h>

//std::string (std::string(" File: ") + std::string(__FILE__) +

//#define __FUN(__s) std::string( \
//			  std::string(__s) std::string( "Function: ") + std::string ( __FUNCTION__ ) + std::string ( ";Line: ") + std::to_string(__LINE__) + std::string (";)")).c_str()
//#define FLN			std::string (std::string( "Function: ") + std::string ( __FUNCTION__ ) + std::string ( ";Line: ") + std::to_string(__LINE__) + std::string (";)"))
//#define FLN2		std::string (std::string("Function: ") + std::string ( __FUNCTION__ ) + std::string ("\nLine: ") + std::to_string(__LINE__) + std::string ("\n"))

//#define LOG_CATCH(_s) catch(std::exception& exc){SendDebug(std::string((_s)) + exc.what());}catch(...){SendDebug(std::string((_s)) + "Unknown error");}


#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "libxl.h"

#include <opc/ua/client/client.h>
#include <opc/ua/client/binary_client.h>

#include <opc/ua/node.h>
#include <opc/ua/server_operations.h>
#include <opc/ua/services/services.h>
#include <opc/ua/subscription.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/channel.h>
#include <opc/common/uri_facade.h>
#include <opc/common/logger.h>


#pragma comment (lib,"Gdiplus.lib")


//#ifdef NDEBUG
//#pragma comment(lib, "opcuaclient_64_Release.lib")
//#pragma comment(lib, "opcuacore_64_Release.lib")
//#pragma comment(lib, "opcuaprotocol_64_Release.lib")
//#else
//#pragma comment(lib, "opcuaclient_64_Debug.lib")
//#pragma comment(lib, "opcuacore_64_Debug.lib")
//#pragma comment(lib, "opcuaprotocol_64_Debug.lib")
//#endif


enum MSSEC {
	sec00000  = 0,
	sec00100  = 100,
	sec00250  = 250,
	sec00500  = 500,
	sec01000  = 1000,
	sec02000  = 2000,
	sec05000  = 5000,
	SPKsec5  = 10000,
	sec15000  = 15000,
	sec20000  = 20000,
	sec30000  = 50000,
	sec60000  = 60000,
};


#include "framework.h"
#include "resource.h"


namespace std
{
	typedef std::vector<boost::filesystem::path> Pathc;
};
