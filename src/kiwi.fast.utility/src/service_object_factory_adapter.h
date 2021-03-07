#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/service_object_factory.h>
#include <kiwi.fast.utility/src/service_object_factory_imp.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_object_factory_adapter : public KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory, public service_object_factory_imp
{
public:
    static service_object_factory_adapter* instance()
    {
        if(m_instance == nullptr)
        {
            if(m_destroyed)
            {
                //抛出异常logic_error   service_command_line_adapter has been destroyed
            }
            m_instance = new service_object_factory_adapter;
        }
        return m_instance;
    }

    static void destroy()
    {
        if(m_destroyed)
        {
            //抛出异常 logic_error service_configuration_setting has been destroyed
        }
        delete m_instance;
        m_instance = nullptr;
        m_destroyed = true;
    }

    SERVICE_OBJECT_FACTORY_ADAPTER_METHOD(service_object_factory_imp)

protected:
    service_object_factory_adapter()
    {}

    virtual ~service_object_factory_adapter()
    {
        //触发信号，对象已经销毁   notify(SIGNAL_NAME(subject, deleted));
    }

private:
    static service_object_factory_adapter* m_instance;
    static bool m_destroyed;
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
