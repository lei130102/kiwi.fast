#include <kiwi.fast.plugin_utility/ptree_item.h>

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

std::shared_ptr<base_ptree_item> base_ptree_item::create_ptree_item(name_type const& name, data_value const& value)
{
    return std::shared_ptr<base_ptree_item>(new ptree_item(name, value));
}

std::shared_ptr<base_ptree_item> base_ptree_item::create_ptree_item_set(name_type const& name)
{
    return std::shared_ptr<base_ptree_item>(new ptree_item_set(name));
}

std::shared_ptr<base_ptree_item> base_ptree_item::create_ptree_root(name_type const& name)
{
    return std::shared_ptr<base_ptree_item>(new ptree_root(name));
}

std::shared_ptr<base_ptree_item> base_ptree_item::from_ptree(ptree_type const& tree)
{
    name_type name;
    type_type type;
    base_ptree_item::from_ptree_info(tree, name, type);
    if (type != base_ptree_item::set_v_type())
    {
        auto iter = tree.find(to_utf8_string(u8"value"));
        std::u8string value = from_utf8_string<char8_t>(iter->second.data());

        /*           service<service_data_value> data_value_service;
                   data_value dv = data_value_service->create_data_value(type);
                   data_value_service->data_value_from_string(value, type, dv); */
        std::shared_ptr<base_ptree_item> item(new ptree_item(name, data_value::from_string(value, type)));

        return item;
    }
    else if (type == base_ptree_item::set_v_type())
    {
        auto iter = tree.find(to_utf8_string(u8"value"));

        std::shared_ptr<base_ptree_item> item(new ptree_item_set(name));
        for (auto const& child_tree : iter->second)
        {
            if (child_tree.first == to_utf8_string(u8"item"))
            {
                std::shared_ptr<base_ptree_item> sub_item = from_ptree(child_tree.second);
                item->add(sub_item);
            }
        }
        return item;
    }
}

std::shared_ptr<base_ptree_item> base_ptree_item::from_root_ptree(ptree_type const& tree)
{
    std::shared_ptr<base_ptree_item> root(new ptree_root(u8""));

    ptree_type kiwi_fast_tree = tree.get_child(to_utf8_string(u8"kiwi_fast"));

    for (auto const& child_tree : kiwi_fast_tree)
    {
        if (child_tree.first == to_utf8_string(u8"item"))
        {
            std::shared_ptr<base_ptree_item> item = base_ptree_item::from_ptree(child_tree.second);
            root->add(item);
        }
    }

    return root;
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE