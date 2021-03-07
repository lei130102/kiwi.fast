#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/manager_external_interface.h>

#include <functional>
#include <algorithm>
#include <string>
#include <deque>

#include <cstdlib>

#define MODULE_VERSION 0

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class plugin;

struct dll_so_module_descriptor
{
    int version;
    const char8_t* module_name;
    const char8_t* function_name;
};

using get_dll_so_module_descriptor_type = dll_so_module_descriptor*(*)(manager_external_interface*);   //函数指针类型
//using get_dll_so_module_descriptor_type = std::function<dll_so_module_descriptor*(manager_external_interface*)>;

using instantiate_plugin_type = bool(*)(manager_external_interface*, std::size_t, plugin**);          //函数指针类型
//using instantiate_plugin_type = std::function<bool(manager_external_interface*, unsigned int, plugin**)>;

class plugin_factory
{
public:
    plugin_factory()
    {}

    virtual ~plugin_factory()
    {}

    virtual plugin* create_plugin(const char8_t* param) = 0;

    /*!
     * \brief param
     * \return
     */
    virtual std::deque<std::u8string> param() = 0;
};

class basic_plugin_factory : public plugin_factory
{
public:
    basic_plugin_factory(const char8_t* param)
        :plugin_factory()
    {
        add_param(param);
    }

    virtual ~basic_plugin_factory()
    {}

    std::deque<std::u8string> param() override
    {
        return m_param;
    }

private:
    basic_plugin_factory()
    {}

    void add_param(const char8_t* param)
    {
        m_param.push_back(param);
    }

    std::deque<std::u8string> m_param;
};

bool operator>(std::pair<std::u8string, plugin_factory*> lhs, std::pair<std::u8string, plugin_factory*> rhs)
{
    if(lhs.second == nullptr || rhs.second == nullptr)
    {
        return false;
    }
    std::size_t lhs_count = std::count(lhs.first.begin(), lhs.first.end(), u8'_');
    std::size_t rhs_count = std::count(rhs.first.begin(), rhs.first.end(), u8'_');
    if(lhs_count != rhs_count)
    {
        return lhs_count > rhs_count;
    }
    return lhs.first > rhs.first;
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define REGISTER_PLUGIN(module_namespace__, plugin_factory_class_name_prefix__, plugin_class_name_, param__)                       \
    namespace module_namespace__                                                                                                   \
    {                                                                                                                              \
        void add_factory(plugin_factory*);                                                                                         \
        class plugin_factory_class_name_prefix__##_plugin_factory : public basic_plugin_factory                                    \
        {                                                                                                                          \
        public:                                                                                                                    \
            plugin_factory_class_name_prefix__##_plugin_factory(const char8_t* param)                                              \
                : basic_plugin_factory(param)                                                                                      \
            {                                                                                                                      \
                module_namespace__::add_factory(this);                                                                             \
            }                                                                                                                      \
            virtual ~plugin_factory_class_name_prefix__##_plugin_factory()                                                         \
            {}                                                                                                                     \
            plugin* create_plugin(const char8_t* name)                                                                             \
            {                                                                                                                      \
                return new plugin_class_name__;                                                                                    \
            }                                                                                                                      \
        };                                                                                                                         \
        static plugin_factory_class_name_prefix__##_plugin_factory plugin_factory_class_name_prefix__##plugin_factory(#param__);   \
    }

#define REGISTER_PLUGIN_BASIC(module_namespace__, plugin_class_name__)                                                             \
    REGISTER_PLUGIN(module_namespace__, plugin_class_name__, plugin_class_name__, plugin_class_name__)

#define GENERATE_FACTORY(module_namespace___)                                                                                      \
    namespace module_namespace__                                                                                                   \
    {                                                                                                                              \
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_USE                                                                                     \
        std::deque<std::pair<std::u8string, plugin_factory*>>& factory()                                                           \
        {                                                                                                                          \
            static std::deque<std::pair<std::u8string, plugin_factory*>> factory_;                                                 \
            return factory_;                                                                                                       \
        }                                                                                                                          \
        void add_factory(plugin_factory* factory_)                                                                                 \
        {                                                                                                                          \
            if(factory_ != nullptr)                                                                                                \
            {                                                                                                                      \
                std::deque<std::u8string> param_ = factory_->param();                                                              \
                std::foreach(param_.begin(), param_.end()                                                                          \
                             ,[](std::u8string const& element){factory().push_back(std::make_pair(element, factory_))});           \
            }                                                                                                                      \
        }                                                                                                                          \
        plugin* plugin(std::size_t plugin_index)                                                                                   \
        {                                                                                                                          \
            static bool factory_sorted = false;                                                                                    \
            if(!factory_sorted)                                                                                                    \
            {                                                                                                                      \
                factory_sorted = true;                                                                                             \
                std::sort(factory().begin(), factory().end(), std::greater<std::pair<std::u8string, plugin_factory*>>);            \
            }                                                                                                                      \
            if(plugin_index >= factory().size())                                                                                   \
            {                                                                                                                      \
                return nullptr;                                                                                                    \
            }                                                                                                                      \
            std::pair<std::u8string, plugin_factory*> const& plugin_ = factory()[plugin_index];                                    \
            return plugin_.second->create_plugin(plugin_.first);                                                                   \
        }                                                                                                                          \
    }
