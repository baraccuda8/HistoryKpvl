#include "pch.h"
//#include "main.h"
////#include "file.h"
////#include "exel.h"
//#include "win.h"
////#include "ValueTag.h"
//#include "ip.h"
//#include "udpclient.h"
//#include "Compressor.h"
//
//#pragma comment(lib, "Compressor.lib")
//
////using namespace boost::adaptors;
//
//
////namespace my{
////Список пкеременных
////    std::map<ULONG, vartype>VarMaps;
////};
//
//ResivHeder HELLO ={VERIP, command::helo, 0, 0};
//ResivHeder GETALLTAG ={VERIP, command::getalltag, 0, 0};
//ResivHeder OK ={VERIP, command::ok, 0, 0};
//ResivHeder END ={VERIP, command::end, 0, 0};
//
//
//
//DWORD TCP_Server(LPVOID);
//DWORD UDP_Server(LPVOID);
//
//BOOL initWSA = false;
//char* SendDataAllTag = NULL;
//ULONG SendDataAllTagLen = SENDDATALEN;
//
//
//std::map<HANDLE, std::string> Vhandle;
//
//
//std::string GetStrError()
//{
//    char* s = NULL;
//    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//                  NULL, WSAGetLastError(),
//                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//                  (LPSTR)&s, 0, NULL);
//    std::string ss = s;
//    LocalFree(s);
//    return ss;
//}
//
//char* SendData = NULL;
//ULONG SendDataLen = SENDDATALEN;
//
//
//void InitIp()
//{
//    WSADATA ws;
//    if(WSAStartup(MAKEWORD(2, 2), &ws) != 0)
//        throw std::exception(__FUN("Ошибка WSAStartup"));
//    initWSA = true;
//
//    //TAG_PLC_KPVL.InitPatch("|var|PLC210 OPC-UA");
//    //TAG_PLC_SPK1.InitPatch("|var|SPK107 (M01)");
//    //int size = 16 + 16 * (int)VAllValue.size() * 3 + 1;
//    //for(auto h : VAllValue)
//    //    size += (int)h.first.length() + 10;
//    //std::stringstream streams;
//    //streams << "Size Buff:" << std::setw(5) << size << '\n';
//    //streams << "Count Var:" << std::setw(5) << static_cast<int32_t>(VAllValue.size()) << '\n';
//    //for(auto h : VAllValue | indexed(1))
//    //{
//    //    streams << "Var Num:" << std::setw(7) << h.index() << '\n';
//    //    streams << "Var Type:" << std::setw(6) << h.value().second << "\n";
//    //    streams << "Var Len:" << std::setw(7) << h.value().first.length() << "\n";
//    //    streams << "Var name:" << h.value().first.c_str() << '\n';
//    //}
//    //streams << '\0';
//    //SendData = (char*)GlobalAllocPtr(GMEM_ZEROINIT, SENDDATALEN);
//    //int level = 9;
//    //compress2((Byte*)SendData, &SendDataLen, (Byte*)streams.str().c_str(), (ULONG)streams.str().length(), level);
//
//}
//
//#define INVALID_HANDLE_VALUE2 ((HANDLE)(LONG_PTR)-2)
//
////void OpenClientTCP()
////{
////    //if(!initWSA)
////    //    InitIp();
////    //HANDLE h = CreateThread(0, 0, TCP_Server, NULL, 0, 0);
////    //Vhandle[h] = "TCP_Server";
////}
////
////void OpenClientUDP()
////{
////    if(!initWSA)
////        InitIp();
////    HANDLE h = CreateThread(0, 0, UDP_Client, NULL, 0, 0);
////    Vhandle[h] = "TCP_Server";
////}
//
//
//void StopClient()
//{
//    std::string ss = "";
//    HANDLE hh = 0;
//    for(auto h : Vhandle)
//    {
//        hh = h.first;
//        ss = h.second;
//        HANDLE e1 = INVALID_HANDLE_VALUE;
//        HANDLE e2 = INVALID_HANDLE_VALUE2;
//        if(h.first != e1 && h.first != e2) //INVALID_HANDLE_VALUE)
//        {
//            WaitForSingleObject(h.first, INFINITE);
//            CloseHandle(h.first);
//        }
//    }
//
//    Vhandle.erase(Vhandle.begin(), Vhandle.end());
//}
//
