#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/code_conversion.h>

#include <kiwi.fast.plugin_utility/ptree_item.h>
#include <kiwi.fast.plugin_utility/ptree_visitor.h>

#include <string>
#include <any>
#include <variant>
#include <deque>
#include <algorithm>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

/*!
 * \brief The ptree_root struct
 * 根，代表一个属性树信息，配合ptree_root::ptree_type使用，这个功能没有计划做的功能很全面，只是满足基本使用就可以了，再高级的已插件的方式提供其他的实现
 *
 * 规定ptree中std::string存放的是UTF-8编码
 */
class ptree_root
{
    friend class ptree_visitor;

public:
    using ptree_type = ptree_item::ptree_type;

    using name_type = ptree_item::name_type;
    using type_type = ptree_item::type_type;
    using value_type = ptree_item::value_type;

    ptree_root()
    {}

    ptree_root(ptree_root const& rhs) = delete;
    ptree_root& operator=(ptree_root const& rhs) = delete;

    ptree_root(ptree_root&& rhs)
    {}

    ptree_root& operator=(ptree_root&& rhs)
    {
        if(this == &rhs)
        {
            return *this;
        }
        return *this;
    }

    void add(ptree_item&& ptree_item_)
    {
        ptree_items.push_back(std::move(ptree_item_));
    }

    void remove(name_type const& name)
    {
        ptree_items.erase(std::remove_if(ptree_items.begin(), ptree_items.end(), [&](auto& element){
            if(element.m_name == name)
            {
                return true;
            }
            else
            {
                return false;
            }
        }), ptree_items.end());
    }

    template<typename CharType>
    ptree_visitor item(CharType const* name)
    {
        ptree_visitor visitor(this);
        visitor.add_name(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name));
        return visitor;
    }

    ptree_type to_ptree() const
    {
        ptree_type kiwi_fast_tree;

        std::for_each(ptree_items.begin(), ptree_items.end(), [&](ptree_item const& ptree_item_){
            kiwi_fast_tree.add_child(to_utf8_string(u8"item"), ptree_item_.to_ptree());
        });

        ptree_type root_tree;
        root_tree.put_child(to_utf8_string(u8"kiwi_fast"), kiwi_fast_tree);

        return root_tree;
    }

    static ptree_root from_ptree(ptree_type const& tree)
    {
        ptree_root root;

        ptree_type kiwi_fast_tree = tree.get_child(to_utf8_string(u8"kiwi_fast"));

        for(auto const& child_tree : kiwi_fast_tree)
        {
            if(child_tree.first == to_utf8_string(u8"item"))
            {
                ptree_item item = ptree_item::from_ptree(child_tree.second);
                root.ptree_items.push_back(item);
            }
        }

        return root;
    }

    std::deque<ptree_item>& items()
    {
        return ptree_items;
    }

private:
    std::deque<ptree_item> ptree_items;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
