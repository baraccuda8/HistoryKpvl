#pragma once
#include "Subscript.h"

#define CATCHINIT() catch(std::runtime_error& exc){ LOG_ERROR(Logger, "{:90}| ", FUNCTION_LINE_NAME, exc.what()); throw std::runtime_error(exc); } catch(...) { LOG_ERROR(Logger, "{:90}| Unknown error", FUNCTION_LINE_NAME); throw; }


class ChannelSubscription;

#define NEWS 0
class Client: public OpcUa::UaClient, public OpcUa::SubscriptionHandler
{
public:

    bool InitGoot = FALSE;
    bool WatchDog = false;
    int WatchDogWait = 0;

    int countconnect1 = 0;
    int countconnect2 = 0;

    HWNDCLIENT w1;
    std::map< MSSEC, std::vector<OpcUa::ReadValueId>> avid;
    std::map<MSSEC, ChannelSubscription>css;

    OpcUa::Node nodeCurrentTime;                //Node Текущее время сервера

    std::string Uri = "";
    std::string NamePLC = "";
    std::string TimePLC = "";
    uint16_t NamespaceIndex = 0;
    std::shared_ptr<spdlog::logger> Logger = NULL;

    int countget = 1;
    time_t SekRun = 0;
    

    Client(std::string uri, std::shared_ptr<spdlog::logger>& logger): Uri(uri), Logger(logger), OpcUa::UaClient(logger)
    { 
    };

    ~Client();

    void Connect();

    void GetRoot();
    void GetTimeServer();
    void GetNameSpace();

    OpcUa::Node GetNode(OpcUa::NodeId& nodeid);
    OpcUa::Node GetNode(OpcUa::ObjectId Id);
    OpcUa::Node GetNode(std::string name);
    
    virtual void Run(int countconnect) = 0;
    virtual void InitNodeId() = 0;
    virtual void InitTag() = 0;
    
    virtual void DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr) = 0;
};

