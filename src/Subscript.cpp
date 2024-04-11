#include "pch.h"

#include "main.h"
#include "win.h"
#include "file.h"
//#include "ValueTag.h"
#include "Subscript.h"


void ChannelSubscription::Create(std::shared_ptr<OpcUa::UaClient>client, Subscriptions* subscript, int ms, std::shared_ptr<spdlog::logger>& logger)
{
    LOG_INFO(Logger, "{:90}| -->CreateSubscription {}", FUNCTION_LINE_NAME, ms);
    if(!isRun)
        throw std::runtime_error((std::string("Завершение работы")).c_str());

    msec = ms;
    try
    {
        handle.erase(handle.begin(), handle.end());
        if(sub.get())
            sub.reset();

        sub = client->CreateSubscription(msec, *subscript);
    }
    catch(std::runtime_error& exc)
    {
        LOG_ERROR(Logger, "{:90}| Error {}", FUNCTION_LINE_NAME, exc.what());
        throw std::runtime_error(exc);
    }
    catch(...)
    {
        LOG_ERROR(Logger, "{:90}| Unknown error {}", FUNCTION_LINE_NAME);
        throw;
    }

    if(!sub)
        throw std::runtime_error(__FUN("Error CreateSubscription: " + std::to_string(msec) + " ms."));

    LOG_INFO(Logger, "{:90}| <--CreateSubscription {}", FUNCTION_LINE_NAME, ms);
}


void ChannelSubscription::Delete()
{
    //try
    //{
    //    //if(sub) sub->UnSubscribe(handle);
    //    //if(sub) sub->Delete();
    //}
    //catch(std::runtime_error& exc)
    //{
    //    LOG_ERROR(Logger, "{:90}| Error {}", FUNCTION_LINE_NAME, exc.what());
    //    throw std::runtime_error(exc);
    //}
    //catch(...)
    //{
    //    LOG_ERROR(Logger, "{:90}| Unknown error {}", FUNCTION_LINE_NAME);
    //    throw;
    //}
    try
    {
        sub.reset();
    }
    catch(std::runtime_error& exc)
    {
        LOG_ERROR(Logger, "{:90}| Error {}", FUNCTION_LINE_NAME, exc.what());
        throw std::runtime_error(exc);
    }
    catch(...)
    {
        LOG_ERROR(Logger, "{:90}| Unknown error {}", FUNCTION_LINE_NAME);
        throw;
    }

    handle.erase(handle.begin(), handle.end());
    sub = NULL;
}

ChannelSubscription::~ChannelSubscription()
{
    Delete();
}

uint32_t ChannelSubscription::Subscribe(OpcUa::Node node)
{
    LOG_INFO(Logger, "{:90}| Subscribe msec: {} ms. {}" + msec, node.ToString());

    if(!isRun)
        throw std::runtime_error(__FUN("Завершение работы"));

    if(!sub)
        throw std::runtime_error(__FUN("Error Subscribe: " + std::to_string(msec) + " ms."));

    if(node.IsValid())
    {
        uint32_t id = 0;
        try
        {
            OpcUa::Variant val = node.GetValue();
            id  = sub->SubscribeDataChange(node);
        }
        catch(std::runtime_error& exc)
        {
            LOG_ERROR(Logger, "{:90}| Error {}", FUNCTION_LINE_NAME, exc.what());
            throw std::runtime_error(exc);
        }
        catch(...)
        {
            LOG_ERROR(Logger, "{:90}| Unknown error {}", FUNCTION_LINE_NAME);
            throw;
        }

        if(!id)
            throw std::runtime_error(__FUN("Error Subscribe: time=" + std::to_string(msec) + " ms. " + node.ToString()));

        handle.push_back(id);
        return id;
    }

    LOG_INFO(Logger, "{:90}| Subscribe Node not valid, msec: {} ms. {}" + msec, node.ToString());
    throw std::runtime_error(__FUN("Error Subscribe: time=" + std::to_string(msec) + " ms. " + node.ToString()));
    return 0;
}

