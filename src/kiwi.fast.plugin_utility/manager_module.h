#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/manager_external_interface.h>

#include <cstddef>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class plugin;

class manager_module
{
public:
    static manager_module* instance()
    {
        if(m_singleton == nullptr)
        {
            m_singleton = new manager_module;
        }
        return m_singleton;
    }

    static const char8_t* name()
    {
        return m_name;
    }

    static int version()
    {
        return m_version;
    }

    static const char8_t* description()
    {
        return m_description;
    }

    static const char8_t* unique_id()
    {
        return m_unique_id;
    }

    static std::size_t total_plugin();

public:
    void set_external_interface_manager(manager_external_interface* external_interface_manager)
    {
        m_external_interface_manager = external_interface_manager;
    }

    manager_external_interface* external_interface_manager()
    {
        return m_external_interface_manager;
    }

    plugin* plugin(std::size_t plugin_index);

protected:
    manager_module()
        :m_external_interface_manager(nullptr)
    {}

    ~manager_module()
    {}

private:
    manager_external_interface* m_external_interface_manager;

private:
    static const char8_t* m_name;
    static int m_version;
    static const char8_t* m_description;
    static const char8_t* m_unique_id;

    static manager_module* m_singleton;

    class deleter
    {
    public:
        ~deleter()
        {
            if(manager_module::instance())
            {
                delete manager_module::instance();
            }
        }
    };
    static deleter m_deleter;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
