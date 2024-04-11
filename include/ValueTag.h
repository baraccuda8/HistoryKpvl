#pragma once

#include "SQL.h"
#include "ClCodeSys.h"


extern std::shared_ptr<spdlog::logger> AllLogger;

//#define isControllerTest true
//#define VARLOGGER(_s){Log(Codesys ? Codesys->Logger : AllLogger, spdlog::level::level_enum::info, _s); SendDebug(_s);} 
//#define VARCATCH(_s)catch(std::runtime_error& exc){VARLOGGER(_s + exc.what());}catch(...){VARLOGGER(_s + "Unknown error");}


void MySetWindowText(HWND h, const char* ss);

//template<class T>
//class Value;

#define SETALLTAG(_p, _t, _f, _e, _s,  _d) {_t = new Value(_p + #_t, _f, _s, _e, _d)}


class Client;

//template<class T>
class Value{
public:
    //template<class T>DWORD (*myfun)(Value* v);
    typedef DWORD (*myfun)(Value* v);

    PGConnection* Conn;

    long ID = 0;
    std::string Patch;
    std::string strVal;
    std::string oldstrVal;
    std::string Comment;

    time_t DataTime = 0;
    std::string strDataTime = "";

    OpcUa::Variant Val;
    OpcUa::Variant OldVal;
    bool Arhive = true;
    HWNDCLIENT winId = HWNDCLIENT::hNull;
    std::string format = "";
    myfun FunctionCallbak = NULL;

    Client* Codesys = NULL;
    int isNode = 0;
    OpcUa::Node Node = OpcUa::Node();
    OpcUa::NodeId NodeId = OpcUa::NodeId();
    MSSEC Sec =  sec00000;
    float coeff = 1.0;
    float hist = 1.0;

    void InsertVal();

    bool Update = false;
    void UpdateVal();
    void InsertValue();
    void SaveSQL();
    void TestValSQL();
    void GetIdValSQL();

    void SetGraff();



    Value(){};
    Value (const std::string n, HWNDCLIENT hc, myfun fn, PGConnection* conn);
    Value (const std::string n, HWNDCLIENT hc, myfun fn, PGConnection* conn, MSSEC s);
    void InitNodeId(Client* cds);
    OpcUa::Node GetNode();
    OpcUa::VariantType GetType();
    OpcUa::Variant GetValue();
    void Set_Value(OpcUa::Variant var);
    void Set_Value();
    uint32_t handle = 0;
    OpcUa::AttributeId attr;

    std::string GetString();
    std::string GetString(std::string patch);

    void SetVariant(HWNDCLIENT id = HWNDCLIENT::hNull);
    void SetVariant(std::string patch, HWNDCLIENT id = HWNDCLIENT::hNull);
    bool Find(const uint32_t h, const std::string p, const OpcUa::Variant& v, OpcUa::AttributeId a);
    bool Find(const uint32_t h, const OpcUa::Variant& v);
    bool TestNode(Client* cds);

    template<class T>
    T GetVal()
    {
        if(Val.IsNul()) return 0;
        return Val.As<T>();
    }
};

template<class T>
inline T GetVal(Value* value)
{
    if(!value || value->Val.IsNul()) return 0;
    return value->Val.As<T>();
}



typedef struct Data{
    std::string patch;
    OpcUa::Variant val;
}Data;

class ClassDataChange: public Subscriptions
{
public:
    bool InitGoot = FALSE;
    bool WatchDog = false;
    int WatchDogWait = 0;

    std::deque<Data>V_Data;

    virtual void DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr) = 0;
};


