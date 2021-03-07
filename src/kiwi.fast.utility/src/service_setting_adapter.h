#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/service_setting.h>
#include <kiwi.fast.utility/src/service_setting_imp.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_setting_adapter : public KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_setting, public service_setting_imp
{
public:
    static service_setting_adapter* instance()
    {
        if(m_instance == nullptr)
        {
            if(m_destroyed)
            {
                //抛出异常logic_error  service_setting has been destroyed
            }
            m_instance = new service_setting_adapter;
        }
        return m_instance;
    }

    static void destroy()
    {
        if(m_destroyed)
        {
            //抛出异常 logic_erro service_setting has been destroyed
        }
        delete m_instance;
        m_instance = nullptr;
        m_destroyed = true;
    }

    SERVICE_SETTING_ADAPTER_METHOD(service_setting_imp)

protected:
    service_setting_adapter()
    {}

    virtual ~service_setting_adapter()
    {
        //触发信号，对象已经销毁   notify(SIGNAL_NAME(subject, deleted));
    }

private:
    static service_setting_adapter* m_instance;
    static bool m_destroyed;
};


KIWI_FAST_CLOSE_UTILITY_NAMESPACE
