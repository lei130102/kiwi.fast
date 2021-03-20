#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/ptree_item.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

#include <string>
#include <deque>
#include <optional>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class ptree_root;

class ptree_visitor
{
    friend class ptree_root;

public:
    using name_type = ptree_item::name_type;
    using type_type = ptree_item::type_type;
    using value_type = ptree_item::value_type;

    ptree_visitor()
        :m_ptree_root(nullptr)
    {}

    ptree_visitor(ptree_root* ptree_root_)
        :m_ptree_root(ptree_root_)
    {}

    virtual ~ptree_visitor()
    {}

    ptree_visitor& operator()(const char8_t* name)
    {
        add_name(name);
        return *this;
    }

    virtual std::optional<name_type> name()
    {
        auto ptree_item = find_object_value_item(m_names);
        if(ptree_item)
        {
            return (*ptree_item).get().name();
        }
        else
        {
            return {};
        }
    }

    virtual std::optional<type_type> type()
    {
        auto ptree_item = find_object_value_item(m_names);
        if(ptree_item)
        {
            return (*ptree_item).get().type();
        }
        else
        {
            return {};
        }
    }

    template<typename T>
    std::optional<resource_object_factory<T>> value()
    {
        auto result = any_value();
        if (result)
        {
            try {
                return std::any_cast<resource_object_factory<T>>(*result);
            }
            catch (std::bad_any_cast& e) {
                return {};
            }
        }
        else
        {
            return {};
        }
    }

    template<typename T>
    std::optional<resource_deque_factory<T>> value()
    {
        auto result = any_value();
        if (result)
        {
            try {
                return std::any_cast<resource_deque_factory<T>>(*result);
            }
            catch (std::bad_any_cast& e) {
                return {};
            }
        }
        else
        {
            return {};
        }
    }

    template<typename T>
    std::optional<resource_object_factory<T>> value(resource_object_factory<T> const& default_value)
    {
        auto result = any_value();
        if (result)
        {
            try
            {
                return std::any_cast<resource_object_factory<T>>(*result);
            }
            catch (std::bad_any_cast& e)
            {
                return default_value;
            }
        }
        else
        {
            return default_value;
        }
    }

    template<typename T>
    std::optional<resource_deque_factory<T>> value(resource_deque_factory<T> const& default_value)
    {
        auto result = any_value();
        if (result)
        {
            try
            {
                return std::any_cast<resource_deque_factory<T>>(*result);
            }
            catch (std::bad_any_cast& e)
            {
                return default_value;
            }
        }
        else
        {
            return default_value;
        }
    }

    virtual std::optional<value_type> any_value()
    {
        auto ptree_item = find_object_value_item(m_names);
        if (ptree_item)
        {
            return (*ptree_item).get().value();
        }
        else
        {
            return {};
        }
    }

    template<typename T>
    bool set_value(T* value)
    {
        auto ptree_item = find_object_value_item(m_names);
        if(ptree_item)
        {
            ////////销毁之前的
            //////service<service_object_factory> object_factory_service;
            //////object_factory_service->destroy_object((*ptree_item).get().value, (*ptree_item).get().type);

            //设置type和value
            (*ptree_item).get().type() = type_converter::to_string<T>();
            (*ptree_item).get().value() = value;

            return true;
        }
        else
        {
            return false;
        }
    }

protected:

    void add_name(std::u8string const& name)
    {
        m_names.push_back(name);
    }

    std::optional<std::reference_wrapper<ptree_item>> find_object_value_item(std::deque<std::u8string>& names);

private:
    std::deque<std::u8string> m_names;
    ptree_root* m_ptree_root;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
