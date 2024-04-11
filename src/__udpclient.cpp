#include "pch.h"
#include "main.h"
#include "file.h"
#include "exel.h"
#include "win.h"
#include "ip.h"
#include "udpclient.h"
#include "SQL.h"
#include "Compressor.h"
//
//bool inittag = false;
//extern bool isInitPLC_KPVL;
//
//ULONG CountVar = 0;
//
//bool InitGoot = FALSE;
//
//
//#pragma comment(lib,"ws2_32.lib") 
//
////#define BUFLEN 512  // max length of answer
////int PORT = 16080;  // the port on which to listen for incoming data
//int TimeOut = 1;
//
//
//#define MAX_SIZE_BUFF 65535
//
//
//
//class UDPClient{
//    int Port = 16080;
//    SOCKADDR_IN ServerAddr ={0};
//    //int sizeofaddr = sizeof(server);
//    int Server_Socket = 0;
//    std::vector<HANDLE>handle;
//    char* ResvData = NULL;
//    DWORD Frame = 0;
//    bool isHelo = false;
//    bool isAllTag = false;
//
//public:
//    std::string strLastError = "";
//    UDPClient(std::string addr)
//    {
//        ResvData = (char*)GlobalAllocPtr(GMEM_ZEROINIT, zlib::MaxCompress_Size);
//        if(!ResvData)
//            throw std::exception(__FUN("Глобальная ошибка GlobalAllocPtr == NULL"));
//
//        ServerAddr.sin_family = AF_INET;
//        ServerAddr.sin_addr.s_addr = inet_addr(addr.c_str());
//        ServerAddr.sin_port = htons(Port);
//
//        TIMEVAL tout ={0, 10000};
//
//        if((Server_Socket = (int)socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
//            throw std::exception(__FUN("Ошибка socket == INVALID_SOCKET"));
//    }
//    ~UDPClient()
//    {
//        closesocket(Server_Socket);
//    }
//
//    void Run();
//    int Parse(char*, int len);
//    int PrepareTag(char*, int);
//    int PrepareValue(char*, int);
//
//    int ReadBuff(char* buf, int len);
//    int SendBuff(char* buf, int len);
//
//};
//
//
//std::map<int, varmap>VarMap;
//void Arhive(const OpcUa::Node& node, bool arh);
//
//void ItitTag()
//{
//    std::string comand = "SELECT * FROM tag;";
//    PGresult* res = conn_kpvl.PGexec(comand);
//    if(PQresultStatus(res) == PGRES_TUPLES_OK)
//    {
//        int Line = PQntuples(res);//Линий
//        for(int l = 0; l < Line; l++)//Линий
//        {
//            std::string name = conn_kpvl.PGgetvalue(res, l, 3);
//            std::string ff = conn_kpvl.PGgetvalue(res, l, 5);
//            Arhive(name, bool(ff == "t"));
//
//            //1 id,
//            //2 id_name,
//            //3 name,
//            //4 type,
//            //5 arhive,
//            //6 comment"
//        }
//    }
//    else
//    {
//        SendDebug("[GetIdSheet] [error] ", utf8_to_cp1251(PQresultErrorMessage(res)));
//    }
//    PQclear(res);
//}
//
//
//std::string GetStringDataTime()
//{
//    std::string out(50, '\0');
//    time_t st = time(0);
//    std::tm TM;
//    localtime_s(&TM, &st);
//    sprintf_s(&out[0], 50, "%04d-%02d-%02d %02d:%02d:%02d ", TM.tm_year + 1900, TM.tm_mon + 1, TM.tm_mday, TM.tm_hour, TM.tm_min, TM.tm_sec);
//    return out;
//}
//
//#define TAGFILE "tag.txt"
//
//int UDPClient::PrepareValue(char* buf, int len)
//{
//    std::string ss;
//    ss.resize(len);
//    memcpy(&ss[0], buf, len);
//    std::string outData = zlib::decompress(ss);
//
//    //VarMap;
//    //my::VarMap.erase(my::VarMap.begin(), my::VarMap.end());
//    std::vector<std::string>vdata;
//    boost::split(vdata, outData, boost::is_any_of(",")); //(CountVar:)(\\d+)
//    size_t tt = vdata.size();
//    SetWindowText(winmap(hEditDiagnose7), std::to_string(tt - 2).c_str());
//    SetWindowText(winmap(hEditDiagnose9), GetStringDataTime().c_str());
//
//    #ifdef TAGFILE
//    std::ofstream f1(TAGFILE, std::ios::binary | std::ios::out | std::ios::trunc);
//    #endif
//
//    for(auto& sd : vdata)
//    {
//        std::vector<std::string>vsd;
//        boost::split(vsd, sd, boost::is_any_of(":"));
//        if(vsd.size() == 2 && vsd[0] == "Count")
//        {
//            ULONG countvar = atol(vsd[1].c_str());
//            if(CountVar != countvar)
//            {
//                isAllTag = false;
//                return 0;
//            }
//        }
//        else if(vsd.size() == 3 && vsd[0] == "Var")
//        {
//            int t = atol(vsd[1].c_str());
//
//            #ifdef TAGFILE
//            if(f1.is_open()) f1 << VarMap[t].name << " = " << vsd[2] << std::endl;
//            #endif
//
//            //0  :1: 2:3: 4
//            //Var:1:t,
//            dataCangeKPVL.DataChange(t, VarMap[t].name, vsd[2], VarMap[t].id);
//        }
//    }
//    #ifdef TAGFILE
//    if(f1.is_open()) f1.close();
//    #endif
//    ItitTag();
//    //SendDebug("Новых данных: " + std::to_string(vdata.size()));
//    InitGoot = true;
//    return len;
//}
//
//int UDPClient::PrepareTag(char* buf, int len)
//{
//    std::string ss;
//    ss.resize(len);
//    memcpy(&ss[0], buf, len);
//    std::string outData = zlib::decompress(ss);
//
//    std::vector<std::string>vdata;
//    boost::split(vdata, outData, boost::is_any_of(",")); //(CountVar:)(\\d+)
//
//    std::fstream f1("all.csv", std::fstream::binary | std::fstream::out);
//
//    for(auto& sd : vdata)
//    {
//        if(f1.is_open())
//        {
//            std::string hg = sd;
//            boost::replace_all(hg, ":", ";");
//            f1 << hg << std::endl;
//        }
//
//        std::vector<std::string>vsd;
//        boost::split(vsd, sd, boost::is_any_of(":")); //(CountVar:)(\\d+)
//        if(vsd.size() == 2 && vsd[0] == "Count")
//        {
//            CountVar = atol(vsd[1].c_str());
//            SetWindowText(winmap(hEditDiagnose6), vsd[1].c_str());
//            SetWindowText(winmap(hEditDiagnose8), GetStringDataTime().c_str());
//        }
//        else if(vsd.size() == 5 && vsd[0] == "Var")
//        {
//            //0  :1: 2:3: 4
//            //Var:1:10:t:|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.LAM_TE1.AI_eu,
//            int t = atol(vsd[1].c_str());
//            VarMap[t].name = vsd[4];
//            VarMap[t].id = atoi(vsd[2].c_str());
//            //dataCangeKPVL.DataChange(t, vsd[4], vsd[3], atoi(vsd[2].c_str()));
//            //VarMaps[t] = vartype(vsd[4], atol(vsd[2].c_str()), vsd[3]);
//        }
//    }
//    if(f1.is_open())
//        f1.close();
//
//    if(CountVar == VarMap.size())
//    {
//        isAllTag = true;
//        //InitSubscribe();
//        
//        //GetSubscribe();
//
//        SendDebug("[UDPClient::PrepareTag] [debug]", "Получили структуру тегов");
//
//        return len;
//    }
//    VarMap.erase(VarMap.begin(), VarMap.end());
//    return 0;
//}
//
//int UDPClient::Parse(char* buf, int len)
//{
//    int total = 0;
//    while(total < len)
//    {
//        ResivHeder* rh = (ResivHeder*)&buf[total];
//        if(*rh == HELLO)
//        {
//            total += sizeof(ResivHeder);
//            isHelo = true;
//        }
//        else if(*rh == GETALLTAG)
//        {
//            total += sizeof(ResivHeder);
//            PrepareTag(&buf[total], rh->size);
//            total += rh->size;
//        }
//        else if(*rh == OK)
//        {
//            total += sizeof(ResivHeder);
//            PrepareValue(&buf[total], rh->size);
//            total += rh->size;
//        }
//        else
//        {
//            total++;
//        }
//    }
//    return 0;
//}
//
//int UDPClient::ReadBuff(char* buf, int len)
//{
//    int sizesockaddr = sizeof(sockaddr);
//    int total = 0;
//    int bytesleft = len;
//    int nbytes = 0;
//    ZeroMemory(buf, len);
//
//    struct timeval tv;
//    fd_set rset;
//
//    FD_ZERO(&rset);
//    //int maxfdp1 = Server_Socket + 1;
//    tv.tv_sec = TimeOut;
//    tv.tv_usec = 0;
//
//    FD_SET(Server_Socket, &rset);
//    int nready = select(Server_Socket + 1, &rset, NULL, NULL, &tv);
//    if(FD_ISSET(Server_Socket, &rset))
//    {
//        //while(isRun && total < len)
//        {
//
//            nbytes = recvfrom(Server_Socket, buf + total, len, 0, (sockaddr*)&ServerAddr, &sizesockaddr);
//            if(nbytes <= 0) return nbytes;
//            len -= nbytes;
//            total += nbytes;
//        }
//    }
//    return total;
//}
//
//int UDPClient::SendBuff(char* buf, int len)
//{
//    ResivHeder* rh = (ResivHeder*)buf;
//    rh->ver = VERIP;
//    rh->frame = ++Frame;
//    rh->size = len - sizeof(ResivHeder);
//    int sizesockaddr = sizeof(sockaddr);
//    int total = 0;
//    int bytesleft = len;
//    int nbytes = 0;
//
//    while(total < len)
//    {
//        nbytes = sendto(Server_Socket, buf + total, len, 0, (sockaddr*)&ServerAddr, sizesockaddr);
//
//        if(nbytes == -1) nbytes;
//        total += nbytes;
//        bytesleft -= nbytes;
//    }
//    return total;
//}
//
//
//void UDPClient::Run()
//{
//    int len = 0;
//    while(isRun)
//    {
//        if(!isHelo)
//        {
//            SendBuff((char*)&HELLO, sizeof(ResivHeder));
//        }
//        else
//        {
//            if(!isAllTag)
//            {
//                SendBuff((char*)&GETALLTAG, sizeof(ResivHeder));
//            }
//            else
//            {
//                SendBuff((char*)&OK, sizeof(ResivHeder));
//            }
//        }
//        if((len = ReadBuff(ResvData, zlib::MaxCompress_Size)) > 0)
//        {
//            Parse(ResvData, len);
//        }
//        Sleep(1000);
//    }
//}
//
//
//DWORD UDP_Client(LPVOID)
//{
//    while(isRun && !isInitPLC_KPVL)
//        Sleep(100);
//
//    while(isRun)
//    {
//        TRYIP
//        {
//            UDPClient srv("192.168.9.60");
//            srv.Run();
//            Sleep(1);
//        }
//        CATCHIP;
//        Sleep(1);
//    }
//    return 0;
//
//    return 0;
//}
//
//
