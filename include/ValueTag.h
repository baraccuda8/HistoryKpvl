#pragma once

#include "SQL.h"
#include "ClCodeSys.h"


extern std::shared_ptr<spdlog::logger> AllLogger;



#define SETALLTAG(_p, _t, _f, _e, _s,  _d) {_t = new Value(_p + #_t, _f, _s, _e, _d)}

std::string ToString(const OpcUa::DateTime& t);

time_t ToTimeT(OpcUa::DateTime dateTime);


class Client;

class Value{
public:
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
    OpcUa::Node Node = OpcUa::Node();
    OpcUa::NodeId NodeId = OpcUa::NodeId();
    MSSEC Sec =  sec00000;
    float coeff = 1.0;
    float hist = 1.0;
	uint32_t handle = 0;
	//OpcUa::AttributeId attr;

    void InsertVal();

    bool Update = false;
    void UpdateVal();
    void InsertValue(std::string& Create_at);
    void SaveSQL();
    void TestValSQL();
    void GetIdValSQL();

    void SetGraff();

	//Value(){};
    Value (const std::string n, HWNDCLIENT hc, myfun fn, PGConnection* conn, MSSEC s = sec01000);
    Value (const std::string n, HWNDCLIENT hc, myfun fn, PGConnection* conn, bool ar, float co, float hi, MSSEC s, std::string fo, OpcUa::Variant v, std::string com);
    void InitNodeId(Client* cds);
    OpcUa::NodeId GetId();
    OpcUa::Node GetNode();
    OpcUa::VariantType GetType();
    OpcUa::Variant GetValuew();
    OpcUa::Variant GetValue();
    void Set_Value(OpcUa::Variant var);
    void Set_Value();
    
    std::string GetString();
    bool GetBool();
    int GetInt();
    float GetFloat();
    double GetDouble();
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


typedef std::deque<Value*> MAP_VALUE;

void MySetWindowText(HWND h, const char* ss);
void MySetWindowText(HWND h, std::string ss);
void MySetWindowText(Value* value);

