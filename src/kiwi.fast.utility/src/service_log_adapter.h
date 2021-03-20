#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/service_log.h>
#include <kiwi.fast.utility/src/service_log_imp.h>

#include <kiwi.fast.plugin_utility/exceptions.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_log_adapter : public KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_log, public service_log_imp
{
public:
    static service_log_adapter* instance()
    {
        if(m_instance == nullptr)
        {
            if(m_destroyed)
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"service_log_adapter 已被销毁");
            }
            m_instance = new service_log_adapter;
        }
        return m_instance;
    }

    static void destroy()
    {
        if(m_destroyed)
        {
            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"service_log_adapter 已被销毁");
        }
        delete m_instance;
        m_instance = nullptr;
        m_destroyed = true;
    }

    SERVICE_LOG_ADAPTER_METHOD(service_log_imp)

protected:
    service_log_adapter(){}

    virtual ~service_log_adapter()
    {
        //触发信号，对象已经销毁   notify(SIGNAL_NAME(subject, deleted));
    }

private:
    static service_log_adapter* m_instance;
    static bool m_destroyed;
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
