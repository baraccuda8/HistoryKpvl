#pragma once
extern bool isRun;

#define NEW 0

//void SendDebug(std::string ss);

//#define SUB_LOG(_s) Log(Logger, spdlog::level::level_enum::info, _s)

#if NEW 
#define CREATESUBSCRIPT(_c, _s, _d, _h) _c[_s]->Create(client, _d, _s, _h);
#define OPCCLIENT std::shared_ptr<OpcUa::UaClient>
#else
#define CREATESUBSCRIPT(_c, _s, _d, _h) _c[_s].Create(client, _d, _s, _h);
#define OPCCLIENT OpcUa::UaClient*
#endif





class Subscriptions: public OpcUa::SubscriptionHandler
{
public:
    virtual void DataChange(uint32_t, const OpcUa::Node&, const OpcUa::Variant&, OpcUa::AttributeId) = 0;
};

class ChannelSubscription
{
public:
    std::vector <uint32_t> handle ={};
    OpcUa::Subscription::SharedPtr sub = NULL;
    int msec = sec01000;
    std::shared_ptr<spdlog::logger> Logger = NULL;

    void Create(OpcUa::UaClient& client, Subscriptions& subscript, int ms, std::shared_ptr<spdlog::logger>& logger);
    ~ChannelSubscription();
    uint32_t Subscribe(OpcUa::Node node);
    void Delete();
};

