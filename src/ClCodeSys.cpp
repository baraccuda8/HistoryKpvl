#include "pch.h"
#include "main.h"
#include "file.h"
#include "win.h"

#include "Subscript.h"

#include "ClCodeSys.h"


void Client::GetRoot()
{
    if(!isRun)
        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Завершение работы...")).c_str());

    LOG_INFO(Logger, "{:90}| Опрос GetRoot countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);

    auto node = client->GetRootNode();
    if(!node.IsValid())
        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Ложные данные:uri = " + Uri)).c_str());
}


void Client::GetTimeServer()
{
    if(!isRun)
        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Завершение работы")).c_str());
    OpcUa::Node nodeCurrentTime = client->GetNode(OpcUa::ObjectId::Server_ServerStatus_CurrentTime);
    OpcUa::Variant valueCurrentTime = nodeCurrentTime.GetValue();
    TimePLC = valueCurrentTime.ToString();
}

void Client::GetNameSpace()
{
    if(!isRun)
        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Завершение работы")).c_str());

    LOG_INFO(Logger, "{:90}| Опрос GetNameSpace countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);

    OpcUa::Node srv = client->GetNode(OpcUa::ObjectId::Server);
    if(!srv.IsValid())
        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Не найден ObjectId::Server:\nuri = " + Uri)).c_str());

    for(auto node : srv.GetChildren())
    {
        if(!isRun)
            throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Завершение работы")).c_str());

        auto nodeId = node.GetId();
        if(nodeId.IsString())
        {
            std::vector<std::string>VectorString;
            boost::split(VectorString, nodeId.GetStringIdentifier(), boost::is_any_of("|"));
            if(VectorString.size() > 2)
            {
                NamespaceIndex = nodeId.StringData.NamespaceIndex;
                NamePLC = VectorString[2];
                break;
            }
        }
    }
    if(!NamespaceIndex)
        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Не найден NamespaceIndex:\nuri = " + Uri)).c_str());
}

OpcUa::Node Client::GetNode(OpcUa::ObjectId Id)
{
    if(!isRun)
        throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Завершение работы")).c_str());
    try
    {
        if(Id == OpcUa::ObjectId::Null)
            throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Ошибка GetNode: Id == NULL ")).c_str());

        OpcUa::Node node = client->GetNode(Id);
        if(!node.IsValid()) // Ложные данные
            throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Ошибка GetNode: Id = ") + std::to_string((uint32_t)Id)).c_str());
        return node;
    }
    catch(std::runtime_error& exc)
    {
        LOG_INFO(Logger,"{:90}| {}", FUNCTION_LINE_NAME, exc.what());
    }
    catch(...)
    {
        LOG_INFO(Logger, "{:90}| Unknown error", FUNCTION_LINE_NAME);
    }
    return OpcUa::Node();
};

OpcUa::Node Client::GetNode(std::string name)
{
    if(!isRun)
        throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Завершение работы")).c_str());

    try
    {
        OpcUa::Node node;
        node = client->GetNode(OpcUa::NodeId(name, NamespaceIndex));
        if(!node.IsValid())
            throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Ошибка GetNode: Name = ") + name).c_str());
        return node;
    }
    catch(std::runtime_error& exc)
    {
        LOG_INFO(Logger, "{:90}| {}", FUNCTION_LINE_NAME, exc.what());
    }
    catch(...)
    {
        LOG_INFO(Logger, "{:90}| Unknown error", FUNCTION_LINE_NAME);
    }
    return OpcUa::Node();
};


void Client::Connect()
{
    while(isRun)
    {
        std::string oo;
        try
        {
            countconnect2++;
    
            SetWindowText(winmap(w1), (std::to_string(countconnect1) + "." + std::to_string(countconnect2)).c_str());
            oo = ("Connect countconnect = " + std::to_string(countconnect1) + "." + std::to_string(countconnect2));

            LOG_INFO(Logger, "{:90}| Connect countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
            client->Connect(Uri);
            return;
        }
        CATCH(Logger, oo)
        //catch(std::runtime_error& exc)
        //{
        //    LOG_ERROR(Logger, "{:90}| Error {} countconnect = {}.{}", FUNCTION_LINE_NAME, exc.what(), countconnect1, countconnect2);
        //}
        //catch(...)
        //{
        //    LOG_ERROR(Logger, "{:90}| Unknown error countconnect = {}.{}", FUNCTION_LINE_NAME, countconnect1, countconnect2);
        //}
        int f = 30;
        while(isRun && (--f))
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    }
    throw std::runtime_error((FUNCTION_LINE_NAME + std::string(" Завершение работы")).c_str());
}





//void Client::InitNodeId(std::deque<Value*>& TagValue)
//{
//    SUB_LOG(Logger, "Инициализация узлов... countconnect = " + std::to_string(countconnect1));
//    SUB_LOG(Logger, "Проверка Patch = Server_ServerStatus_CurrentTime");
//
//    nodeCurrentTime = GetNode(OpcUa::ObjectId::Server_ServerStatus_CurrentTime); //Node текущее время
//
//    if(nodeCurrentTime.IsValid())
//    {
//        OpcUa::Variant val = nodeCurrentTime.GetValue();
//        //S107::ServerDataTime = val.ToString();
//        //SetWindowText(winmap(hEditTime_2), S107::ServerDataTime.c_str());
//        //SetWindowText(winmap(hEditTime_2), (S107::ServerDataTime + " (" + std::to_string(dataCangeS107.WatchDogWait) + ")").c_str());
//    }
//
//    SUB_LOG(Logger, "Инициализация NodeId");
//    
//    for(auto& a : TagValue)
//    {
//        a->InitNodeId(this);
//    }
//}


//uint32_t Client::Subscribe(MSSEC idSub, OpcUa::Node& node)
//{
//    if(!isRun)
//        throw std::runtime_error((FUNCTION_LINE_NAME + std::string("Завершение работы")).c_str());
//    return  css[idSub].Subscribe(node);
//}
