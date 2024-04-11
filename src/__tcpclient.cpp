#include "pch.h"
//#include "main.h"
//#include "file.h"
//#include "exel.h"
//#include "win.h"
//#include "tcpclient.h"
//
//
/*
#ifdef _DEBUG
    #pragma comment(lib, "Debug\\zlib.lib")
#else
    #pragma comment(lib, "Release\\zlib.lib")
#endif

#define HELO "Helo KPVL Data!"
#define GETALLTAG "Get All Tag"
#define OK "Ok"

std::map<HANDLE, std::string> VhandleTCP;
BOOL initWSA = false;


#define SENDDATALEN 65535
char* SendData = NULL;
ULONG SendDataLen = SENDDATALEN;

char* UncompressData = NULL;
ULONG UncompressDataLen = SENDDATALEN;


class TCPClient{
    int Port = 16080;
    SOCKADDR_IN ClientAddr ={0};
    int Socket = 0;
    std::vector<HANDLE>handle;

public:
    TCPClient();
    ~TCPClient();
    void Run();
    //static DWORD Read(LPVOID);
    //static void SendBuff(int Client, const char* buf, int len);
};

void SendBuff(int ClientSocket, const char* buf, int len)
{
    int total = 0;        // как много байт мы отправляем
    int bytesleft = len; // как много байт осталось отправить
    int nbytes = 0;

    while(total < len)
    {
        nbytes = send(ClientSocket, buf + total, bytesleft, 0);
        if(nbytes == -1) break;
        total += nbytes;
        bytesleft -= nbytes;
    }
    return;
}

void SendAllTag(int ServerSocket)
{
    SendBuff(ServerSocket, GETALLTAG, (int)strlen(GETALLTAG));
    int nbytes;
    //char buf[256];
    if((nbytes = recv(ServerSocket, SendData, SendDataLen, 0)) <= 0)
    {
        if(nbytes == 0)
            throw std::exception(__FUN("Close socet"));
        else
            throw std::exception(__FUN("Error socet"));
    }
    
    uncompress((Byte*)UncompressData, &UncompressDataLen, (Byte*)SendData, nbytes);

    SendBuff(ServerSocket, OK, (int)strlen(OK));

    int tt = 0;

}


void SendHelo(int ServerSocket)
{
    SendBuff(ServerSocket, HELO, (int)strlen(HELO));
    int nbytes;
    char buf[256];
    if((nbytes = recv(ServerSocket, buf, sizeof(buf), 0)) <= 0)
    {
        if(nbytes == 0)
            throw std::exception(__FUN("Close socet"));
        else
            throw std::exception(__FUN("Error socet"));
    }
    else if(!memcmp(buf, HELO, nbytes))
        SendAllTag(ServerSocket);
}

static DWORD Read(LPVOID sl)
{
    int ServerSocket = PtrToInt(sl);

    TIMEVAL tout ={0};
    TIMEVAL trcv ={0};
    TIMEVAL tsnd ={0};
    tout.tv_sec = 0;
    tout.tv_usec = 10000;
    int retrcv = sizeof(TIMEVAL);
    int retsnd = sizeof(TIMEVAL);

    if(setsockopt(ServerSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tout, sizeof(TIMEVAL)) < 0)
        throw std::exception(__FUN("Ошибка setsockopt 3 == SOCKET_ERROR"));

    if(setsockopt(ServerSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&tout, sizeof(TIMEVAL)) < 0)
        throw std::exception(__FUN("Ошибка setsockopt 4 == SOCKET_ERROR"));

    SendHelo(ServerSocket);
    while(isRun)
    {
        SendBuff(ServerSocket, OK, (int)strlen(OK));
        Sleep(1000);
    }

}

TCPClient::TCPClient()
{
    //const char* dest = "asasas";
    //char source [256];
    //ULONG deslen = 0;
    //ULONG sourceLen = (ULONG)strlen(dest);
    //int level = 0;
    //compress2((Byte*)dest, &deslen, (Byte*)source, sourceLen, level);
}

TCPClient::~TCPClient()
{
    closesocket(Socket);
}

void TCPClient::Run()
{
    while(isRun)
    {
    }
}

DWORD TCP_Client(LPVOID)
{
    while(isRun)
    {
        try
        {
            TCPClient clr;
            clr.Run();
            Sleep(1000);
        }
        catch(std::exception& exc)
        {
            std::string ss = exc.what();
        }
        catch(...)
        {
            std::string ss = "Unknown error";
        }
        Sleep(5000);
    }
    return 0;
}


void InitTcpIp()
{
    WSADATA ws;
    if(WSAStartup(MAKEWORD(2, 2), &ws) != 0)
        throw std::exception(__FUN("Ошибка WSAStartup"));
    initWSA = true;

    SendData = (char*)GlobalAllocPtr(GMEM_ZEROINIT, SENDDATALEN);
    UncompressData = (char*)GlobalAllocPtr(GMEM_ZEROINIT, SENDDATALEN);
}


void OpenClientTCP()
{
    if(!initWSA)
        InitTcpIp();

    HANDLE h = CreateThread(0, 0, TCP_Client, NULL, 0, 0);
    VhandleTCP[h] = "TCP_Server";

}
*/