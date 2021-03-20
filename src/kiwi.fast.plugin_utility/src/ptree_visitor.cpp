#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/ptree_visitor.h>
#include <kiwi.fast.plugin_utility/ptree_root.h>

#include <string>
#include <deque>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

std::optional<std::reference_wrapper<ptree_item>> ptree_visitor::find_object_value_item(std::deque<std::u8string>& names)
{
    if(names.size() == 0)
    {
        return {};
    }

    if (m_ptree_root == nullptr)
    {
        return {};
    }

    for(auto& ptree_item_ : m_ptree_root->ptree_items)
    {
        if(ptree_item_.name() == names.front())
        {
            if(ptree_item_.value().type() == typeid(std::deque<ptree_item>))
            {
                names.pop_front();
                return find_object_value_item(names);
            }
            else
            {
                return ptree_item_;
            }
        }
    }
    return {};
}
KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
