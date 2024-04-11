#pragma once

//#define TRYIP try
//#define CATCHIP \
//catch(std::exception& exc)\
//{\
//    std::string strLastError = __FUN(GetStrError() + "\n" + exc.what());\
//    MessageBox(NULL, strLastError.c_str(), "Ошибка", 0);\
//}\
//catch(...)\
//{\
//    std::string strLastError = __FUN(GetStrError() + "\n" + "Unknown error");\
//    MessageBox(NULL, strLastError.c_str(), "Ошибка", 0);\
//}
//
//typedef struct _varmap{
//    int i;
//    int id;
//    std::string name;
//    bool arhive;
//}varmap;
//
//
//namespace OpcUa{
//    typedef std::string& Node;
//    typedef std::string& Variant;
//    typedef uint32_t AttributeId;
//};
//
//#define MSSEC int
//
//
//
//#define VERIP 0x3F1FFDC1
//
//
////typedef struct vartype;
////typedef DWORD (*myfun)(vartype* t);
//
////namespace my{
//    //typedef struct vartype{
//    //    //int ID = 0;
//    //    std::string name = "";
//    //    ULONG type = 0;
//    //    std::string strVal = "";
//
//    //    HWNDCLIENT winId = HWNDCLIENT::hNull;
//    //    std::string format = "";
//    //    myfun FunctionCallbak = NULL;
//
//
//    //    void SetVariant(HWNDCLIENT id = HWNDCLIENT::hNull)
//    //    {
//    //        if(!FunctionCallbak)
//    //        {
//    //            if(id != NULL)
//    //                SetWindowText(winmap(id), strVal.c_str());
//
//    //            if(winId != NULL)
//    //                SetWindowText(winmap(winId), strVal.c_str());
//    //        }
//    //        else
//    //            FunctionCallbak(this);
//    //    }
//
//    //    void SetVariant(std::string val, HWNDCLIENT id = HWNDCLIENT::hNull)
//    //    {
//    //        strVal = val;
//    //        SetVariant(id);
//    //    }
//
//
//
//    //    //bool Subscribe(std::string n, myfun F)
//    //    //{
//    //    //    return Subscribe(n, HWNDCLIENT::hNull, "", F);
//    //    //}
//    //    //bool Subscribe(std::string n, HWNDCLIENT id, myfun F)
//    //    //{
//    //    //    return Subscribe(n, id, "", F);
//    //    //}
//    //    //bool Subscribe(std::string n, std::string f)
//    //    //{
//    //    //    return Subscribe(n, HWNDCLIENT::hNull, f, NULL);
//    //    //}
//
//    //    //bool Subscribe(std::string n, HWNDCLIENT id = HWNDCLIENT::hNull, std::string f = "", myfun F = NULL)
//    //    bool Subscribe(std::string n, HWNDCLIENT hid, std::string f, myfun F)
//    //    {
//    //        if(name == "|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SpeedSection.Top")
//    //        {
//    //            if(n == "|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SpeedSection.Top")
//    //            {
//
//    //                int t = 0;
//    //            }
//
//    //        }
//
//    //        if(name == n)
//    //        {
//    //            //ID = id;
//    //            FunctionCallbak = F;
//    //            format = f;
//    //            winId = hid;
//    //            return true;
//    //        }
//    //        return false;
//    //    }
//
//    //    vartype(){};
//    //    vartype(std::string n, ULONG t, std::string V): name(n), type(t), strVal(V){};
//    //};
//
//    //extern std::map<ULONG, vartype>VarMaps;
//    //extern std::map<ULONG, my::vartype>VarMap;
////};
//namespace command{
//    enum{
//        helo = 1,
//        getalltag = 2,
//        ok = 3,
//        end = 4,
//    };
//};
//
//
//typedef struct ResivHeder{
//    DWORD ver = 0;
//    DWORD cmd = 0;
//    DWORD frame = 0;
//    DWORD size = 0;
//    bool operator == (ResivHeder& r)
//    {
//        return r.ver == ver && r.cmd == cmd;
//    }
//    bool operator == (ResivHeder* r)
//    {
//        return r->ver == ver && r->cmd == cmd;
//    }
//};
//
//
//extern ResivHeder HELLO;
//extern ResivHeder GETALLTAG;
//extern ResivHeder OK;
//extern ResivHeder END;
//
//
//
//
//#define SENDDATALEN 65535
//#define TIMEVAL struct timeval
//#define SOCKADDR_IN struct sockaddr_in
//
//extern std::map<HANDLE, std::string> Vhandle;
//extern char* SendDataAllTag;
//extern ULONG SendDataAllTagLen;
//extern std::map<HANDLE, std::string> Vhandle;
//
//std::string GetStrError();
//void OpenClientUDP();
//void OpenClientTCP();
//void StopClient();
//
//
//
