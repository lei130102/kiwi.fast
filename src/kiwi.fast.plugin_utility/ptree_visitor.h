#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/ptree_item.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/ptree_item.h>

#include <string>
#include <deque>
#include <optional>
#include <memory>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class base_ptree_visitor
{
public:
    using name_type = base_ptree_item::name_type;
    using type_type = base_ptree_item::type_type;
    using value_type = ptree_item::value_type;
    using item_value_type = ptree_item::item_value_type;

    virtual ~base_ptree_visitor() {}

    base_ptree_visitor* item(const char8_t* name)
    {
        add_name(name);
        return this;
    }

    void set_name(std::u8string const& name)
    {
        m_names.clear();
        m_names.push_back(name);
    }

    std::deque<name_type> names() const
    {
        return m_names;
    }

    base_ptree_item* ptree_root() const
    {
        return m_ptree_root;
    }

    virtual std::optional<std::reference_wrapper<name_type>> name() = 0;
    virtual std::optional<type_type> type() = 0;
    virtual std::optional<std::reference_wrapper<item_value_type>> value() = 0;

protected:
    base_ptree_visitor(base_ptree_item* ptree_root)
        :m_ptree_root(ptree_root)
    {}

    void add_name(std::u8string const& name)
    {
        m_names.push_back(name);
    }

private:
    std::deque<name_type> m_names;
    base_ptree_item* m_ptree_root;
};

class ptree_visitor : public base_ptree_visitor
{
public:
    ptree_visitor(base_ptree_item* ptree_root)
        : base_ptree_visitor(ptree_root)
    {}

    std::optional<std::reference_wrapper<name_type>> name() override
    {
        std::deque<name_type> names(names());
        auto ptree_item = detail::find_ptree_item_by_names(ptree_root(), names);
        if (ptree_item)
        {
            return (*ptree_item)->name();
        }
        else
        {
            return {};
        }
    }

    std::optional<type_type> type() override
    {
        std::deque<std::u8string> names(names());
        auto ptree_item = detail::find_ptree_item_by_names(ptree_root(), names);
        if(ptree_item)
        {
            return (*ptree_item)->v_type();
        }
        else
        {
            return {};
        }
    }

    std::optional<std::reference_wrapper<item_value_type>> value() override
    {
        std::deque<std::u8string> names(names());
        auto ptree_item = detail::find_ptree_item_by_names(ptree_root(), names);
        if (ptree_item)
        {
            return (*ptree_item)->item_value();
        }
        else
        {
            return {};
        }
    }
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
