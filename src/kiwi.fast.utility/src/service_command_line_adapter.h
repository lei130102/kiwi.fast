#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/service_command_line.h>
#include <kiwi.fast.utility/src/service_command_line_imp.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_command_line_adapter : public KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_command_line, public service_command_line_imp
{
public:
    static service_command_line_adapter* instance()
    {
        if(m_instance == nullptr)
        {
            if(m_destroyed)
            {
                //抛出异常logic_error   service_command_line_adapter has been destroyed
            }
            m_instance = new service_command_line_adapter;
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

    SERVICE_COMMAND_LINE_ADAPTER_METHOD(service_command_line_imp)

protected:
    service_command_line_adapter(){}

    virtual ~service_command_line_adapter()
    {
        //触发信号，对象已经销毁   notify(SIGNAL_NAME(subject, deleted));
    }

private:
    static service_command_line_adapter* m_instance;
    static bool m_destroyed;
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE