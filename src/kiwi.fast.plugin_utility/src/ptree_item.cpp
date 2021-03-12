#include <kiwi.fast.plugin_utility/ptree_item.h>

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

ptree_item::ptree_item(name_type const& name_)
	:name(name_)
	, type(type_converter::to_string<std::deque<ptree_item>>())
    , value(std::deque<ptree_item>())
{}

ptree_item::~ptree_item()
{
    if(value.type() != typeid(std::deque<ptree_item>))
    {
        service<service_object_factory> object_factory_service;
        object_factory_service->destroy_object(value, type);
    }
}

void ptree_item::add(ptree_item const& ptree_item_)
{
    if(value.type() == typeid(std::deque<ptree_item>))
	{
		std::any_cast<std::deque<ptree_item>&>(value).push_back(ptree_item_);
	}
}

void ptree_item::remove(name_type const& name_)
{
    if(value.type() == typeid(std::deque<ptree_item>))
	{
		std::deque<ptree_item>& deque_ = std::any_cast<std::deque<ptree_item>&>(value);
		deque_.erase(std::remove_if(deque_.begin(), deque_.end(), [&](auto& element) {
			if (element.name == name_)
			{
				return true;
			}
			else
			{
				return false;
			}
			}), deque_.end());
	}
}

ptree_item::ptree_type ptree_item::to_ptree() const
{
    ptree_type item_tree;
    item_tree.put(to_utf8_string(u8"name"), to_utf8_string(name));
    item_tree.put(to_utf8_string(u8"type"), to_utf8_string(type));
    if (value.type() == typeid(std::deque<ptree_item>))
    {
        std::deque<ptree_item> const& value_ = std::any_cast<std::deque<ptree_item> const&>(value);

        ptree_type sub_item_tree;

        std::for_each(value_.begin(), value_.end(), [&](ptree_item const& ptree_item_) {
            sub_item_tree.add_child(to_utf8_string(u8"item"), ptree_item_.to_ptree());
            });

        item_tree.put_child(to_utf8_string(u8"value"), sub_item_tree);
    }
    else
    {
        service<service_object_factory> object_factory_service;
        item_tree.put(to_utf8_string(u8"value"),
            to_utf8_string(object_factory_service->object_to_string(value, type)));
    }

    return item_tree;
}

ptree_item ptree_item::from_ptree(ptree_type const& tree)
{
    auto iter = tree.find(to_utf8_string(u8"name"));
    if (iter == tree.not_found())
    {
        //抛出异常
    }
    std::u8string name = from_utf8_string<char8_t>(iter->second.data());

    iter = tree.find(to_utf8_string(u8"type"));
    if (iter == tree.not_found())
    {
        //抛出异常
    }
    std::u8string type = from_utf8_string<char8_t>(iter->second.data());

    iter = tree.find(to_utf8_string(u8"value"));
    if (iter == tree.not_found())
    {
        //抛出异常
    }
    if (type == type_converter::to_string<std::deque<ptree_item>>())
    {
        ptree_item item(name);
        for (auto const& child_tree : iter->second)
        {
            if (child_tree.first == to_utf8_string(u8"item"))
            {
                ptree_item sub_item = from_ptree(child_tree.second);
                std::any_cast<std::deque<ptree_item>&>(item.value).push_back(std::move(sub_item));
            }
        }
        return item;
    }
    else
    {
        std::u8string value = from_utf8_string<char8_t>(iter->second.data());

        service<service_object_factory> object_factory_service;
        auto value_object = object_factory_service->create_object(type);
        object_factory_service->string_to_object(value, value_object, type);
        ptree_item item(name, type, value_object);
        return item;
    }
}




KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE