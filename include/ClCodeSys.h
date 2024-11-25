#pragma once
#include "Subscript.h"

#define CATCHINIT() catch(std::runtime_error& exc)\
    {\
        LOG_ERROR(Logger, "{:90}| ", FUNCTION_LINE_NAME, exc.what());\
        throw std::runtime_error(exc);\
    }\
    catch(...)\
    {\
        LOG_ERROR(Logger, "{:90}| Unknown error", FUNCTION_LINE_NAME);\
        throw;\
    }


//class Client
//{
//public:
//    OpcUa::UaClient client;
//    std::string Uri = "";
//    std::string NamePLC = "";
//    std::string TimePLC = "";
//    uint16_t NamespaceIndex = 0;
//    std::shared_ptr<spdlog::logger> Logger = NULL;
//
//    int countget = 1;
//
//    std::map<MSSEC, ChannelSubscription>css;
//  // ={
//  //  { sec00100, css00100},
//  //  { sec00500, css00500},
//  //  { sec00250, css00250},
//  //  { SPKsec5, css01000},
//  //  { sec02000, css02000},
//  //  { sec05000, css05000},
//  //  { SPKsec5, css10000},
//  //  { sec15000, css15000},
//  //  { sec20000, css20000},
//  //  { sec30000, css30000},
//  //  { sec60000, css60000},
//  //  };
//
//    Client(std::string uri, std::shared_ptr<spdlog::logger> log): Uri(uri){
//        Logger = log;
//        client.SetLoger(Logger);
//    }
//
//    void GetRoot()
//    {
//        if(!isRun)
//            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Завершение работы")).c_str());
//
//        auto node = client.GetRootNode();
//        if(!node.IsValid())
//            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Ложные данные:\nuri = " + Uri)).c_str());
//    }
//
//
//    void GetTimeServer()
//    {
//        if(!isRun)
//            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Завершение работы")).c_str());
//        OpcUa::Node nodeCurrentTime = client.GetNode(OpcUa::ObjectId::Server_ServerStatus_CurrentTime);
//        OpcUa::Variant valueCurrentTime = nodeCurrentTime.GetValue();
//        TimePLC = valueCurrentTime.ToString();
//    }
//
//    void GetNameSpace()
//    {
//        if(!isRun)
//            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Завершение работы")).c_str());
//
//        OpcUa::Node srv = client.GetNode(OpcUa::ObjectId::Server);
//        if(!srv.IsValid())
//            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Не найден ObjectId::Server:\nuri = " + Uri)).c_str());
//
//        for(auto node : srv.GetChildren())
//        {
//            if(!isRun)return;
//            auto nodeId = node.GetId();
//            if(nodeId.IsString())
//            {
//                std::vector<std::string>VectorString;
//                boost::split(VectorString, nodeId.GetStringIdentifier(), boost::is_any_of("|"));
//                if(VectorString.size() > 2)
//                {
//                    NamespaceIndex = nodeId.StringData.NamespaceIndex;
//                    NamePLC = VectorString[2];
//                    break;
//                }
//            }
//        }
//        if(!NamespaceIndex)
//            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Не найден NamespaceIndex:\nuri = " + Uri)).c_str());
//    }
//
//    OpcUa::Node GetNode(OpcUa::ObjectId Id)
//    {
//        if(!isRun)
//            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Завершение работы")).c_str());
//        try
//        {
//            if(Id == OpcUa::ObjectId::Null)
//                throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Ошибка GetNode: Id == NULL ")).c_str());
//
//            OpcUa::Node node = client.GetNode(Id);
//            if(!node.IsValid()) // Ложные данные
//                throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Ошибка GetNode: Id = ") + std::to_string((uint32_t)Id)).c_str());
//            return node;
//        }
//        catch(std::runtime_error& exc)
//        {
//            SendDebug(exc.what());
//        }
//        catch(...)
//        {
//            SendDebug("Unknown error");
//        }
//        return OpcUa::Node();
//    };
//
//    OpcUa::Node GetNode(std::string name)
//    {
//        if(!isRun)
//            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Завершение работы")).c_str());
//
//        try
//        {
//            OpcUa::Node node;
//            node = client.GetNode(OpcUa::NodeId(name, NamespaceIndex));
//            if(!node.IsValid())
//                throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Ошибка GetNode: Name = ") + name).c_str());
//            return node;
//        }
//        catch(std::runtime_error& exc)
//        {
//            SendDebug(exc.what());
//        }
//        catch(...)
//        {
//            SendDebug("Unknown error");
//        }
//        return OpcUa::Node();
//    };
//
//    uint32_t Subscribe(MSSEC idSub, OpcUa::Node node)
//    {
//        if(!isRun)
//            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Завершение работы")).c_str());
//        return  css[idSub].Subscribe(node);
//    }
//};
//

//using namespace OpcUa;
class ClassDataChange: public Subscriptions
{
public:
    bool InitGoot = FALSE;
    bool WatchDog = false;
    int WatchDogWait = 0;

    virtual void DataChange(uint32_t handle, const OpcUa::Node& node, const OpcUa::Variant& val, OpcUa::AttributeId attr) = 0;
};


#define NEWS 0
class Client: public OpcUa::UaClient
{
public:
    int countconnect1 = 0;
    int countconnect2 = 0;

    HWNDCLIENT w1;

    OpcUa::Node nodeCurrentTime;                //Node Текущее время сервера

#if NEWS
    std::shared_ptr<OpcUa::UaClient>client = NULL;
#else
    //OpcUa::UaClient * client = NULL;
#endif
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
    //uint32_t Subscribe(MSSEC idSub, OpcUa::Node& node);
};

