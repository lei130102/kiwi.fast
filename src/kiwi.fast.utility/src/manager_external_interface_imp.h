#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/manager_external_interface.h>

#include <kiwi.fast.utility/src/service_exe_run_mode_adapter.h>
#include <kiwi.fast.utility/src/service_object_factory_adapter.h>
#include <kiwi.fast.utility/src/service_command_line_adapter.h>
#include <kiwi.fast.utility/src/service_setting_adapter.h>

#include <string>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class manager_external_interface_imp : public KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_external_interface
{
public:
    static manager_external_interface_imp* instance()
    {
        if(m_instance == nullptr)
        {
            if(m_destroyed)
            {
                //抛出异常logic_error   manager_external_interface has been destroyed
            }
            m_instance = new manager_external_interface_imp;
        }
        return m_instance;
    }
    static void destroy()
    {
        if(m_destroyed)
        {
            //抛出异常logic_error   manager_external_interface has been destroyed
        }
        delete m_instance;
        m_instance = nullptr;
        m_destroyed = true;
    }

    static void map_manager(manager_external_interface_imp* manager)
    {
        if(m_destroyed)
        {
            //抛出异常logic_error   manager_external_interface has been destroyed
        }
        if((m_instance != nullptr) && (m_instance != manager))
        {
            delete manager;
        }
        m_instance = manager;
    }

public:
    bool query(const char8_t* name, void** address) override
    {
        *address = nullptr;

        if(std::u8string(name) == u8"service_exe_run_mode")
        {
            *address = dynamic_cast<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_exe_run_mode*>(service_exe_run_mode_adapter::instance());
        }
        else if(std::u8string(name) == u8"service_object_factory")
        {
            *address = dynamic_cast<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory*>(service_object_factory_adapter::instance());
        }
        else if(std::u8string(name) == u8"service_command_line")
        {
            *address = dynamic_cast<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_command_line*>(service_command_line_adapter::instance());
        }
        else if(std::u8string(name) == u8"service_setting")
        {
            *address = dynamic_cast<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_setting*>(service_setting_adapter::instance());
        }

        if(*address != nullptr)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    void query_destroy(const char8_t* name) override
    {
        if(std::u8string(name) == u8"service_exe_run_mode")
        {
            service_exe_run_mode_adapter::destroy();
        }
        else if(std::u8string(name) == u8"service_object_factory")
        {
            service_object_factory_adapter::destroy();
        }
        else if(std::u8string(name) == u8"service_command_line")
        {
            service_command_line_adapter::destroy();
        }
        else if(std::u8string(name) == u8"service_setting")
        {
            service_setting_adapter::destroy();
        }
    }

protected:
    manager_external_interface_imp()
    {}

    ~manager_external_interface_imp()
    {}

private:
    static manager_external_interface_imp* m_instance;
    static bool m_destroyed;
};


KIWI_FAST_CLOSE_UTILITY_NAMESPACE
