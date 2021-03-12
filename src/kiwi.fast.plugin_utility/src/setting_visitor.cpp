#include <kiwi.fast.plugin_utility/setting_visitor.h>

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/ptree_root.h>
#include <kiwi.fast.plugin_utility/service_setting.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

std::optional<std::reference_wrapper<ptree_item>> setting_visitor::find_setting_default_object_value_item(std::deque<std::u8string>& names)
{
    if(names.size() == 0)
    {
        return {};
    }

    for(auto& ptree_item_ : setting_service->setting_default().ptree_items)
    {
        if(ptree_item_.name == names.front())
        {
            if (ptree_item_.value.type() == typeid(std::deque<ptree_item>))
            {
                names.pop_front();
                return find_setting_default_object_value_item(names);
            }
            else
            {
                return ptree_item_;
            }
        }
    }
    return {};
}

std::optional<std::reference_wrapper<ptree_item>> setting_visitor::find_setting_user_object_value_item(std::deque<std::u8string>& names)
{
    if(names.size() == 0)
    {
        return {};
    }

    for(auto& ptree_item_ : setting_service->setting_user().ptree_items)
    {
        if(ptree_item_.name == names.front())
        {
            if (ptree_item_.value.type() == typeid(std::deque<ptree_item>))
            {
                names.pop_front();
                return find_setting_user_object_value_item(names);
            }
            else
            {
                return ptree_item_;
            }
        }
    }
    return {};
}

std::optional<std::reference_wrapper<ptree_item>> setting_visitor::find_setting_tmp_object_value_item(std::deque<std::u8string>& names)
{
    if(names.size() == 0)
    {
        return {};
    }

    for(auto& ptree_item_ : setting_service->setting_tmp().ptree_items)
    {
        if(ptree_item_.name == names.front())
        {
            if (ptree_item_.value.type() == typeid(std::deque<ptree_item>))
            {
                names.pop_front();
                return find_setting_tmp_object_value_item(names);
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
