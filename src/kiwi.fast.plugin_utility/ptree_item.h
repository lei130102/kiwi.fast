#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/data_value.h>
#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/service_data_value.h>

#include <boost/property_tree/ptree.hpp>

#include <string>
#include <any>
#include <variant>
#include <deque>
#include <type_traits>
#include <memory>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class base_ptree_item : public std::enable_shared_from_this<base_ptree_item>
{
public:
    //使用ptree，而不是wptree，是因为wptree写入读取文件时使用wiofstream，他在mingw下无法正常工作(解决方式麻烦)
    using ptree_type = boost::property_tree::ptree;

    using name_type = std::u8string;
    using type_type = std::u8string;
    using item_value_type = data_value;
    using item_set_value_type = std::deque<std::shared_ptr<base_ptree_item>>;

    //m_type改到派生类中，当修改data_value时自动更新

    //改为friend非成员函数
    static std::shared_ptr<base_ptree_item> create_ptree_item(name_type const& name, data_value const& value);
    static std::shared_ptr<base_ptree_item> create_ptree_item_set(name_type const& name);
    static std::shared_ptr<base_ptree_item> create_ptree_root(name_type const& name);
    static std::shared_ptr<base_ptree_item> from_ptree(ptree_type const& tree);
    static std::shared_ptr<base_ptree_item> from_root_ptree(ptree_type const& tree);

    virtual ~base_ptree_item() {};

    virtual void add(std::shared_ptr<base_ptree_item> const& ptree_item_) = 0;

    virtual void remove(name_type const& name) = 0;

    virtual ptree_type to_ptree() const = 0;

    virtual ptree_type to_root_ptree() const = 0;

    std::reference_wrapper<name_type> name()
    {
        return m_name;
    }

    std::reference_wrapper<const name_type> name() const
    {
        return m_name;
    }

    std::reference_wrapper<type_type> v_type()
    {
        return m_type;
    }

    std::reference_wrapper<const type_type> v_type() const
    {
        return m_type;
    }

    virtual std::optional<std::reference_wrapper<item_value_type>> item_value() = 0;

    virtual std::optional<std::reference_wrapper<const item_value_type>> item_value() const = 0;

    virtual std::optional<std::reference_wrapper<item_set_value_type>> item_set_value() = 0;

    virtual std::optional<std::reference_wrapper<const item_set_value_type>> item_set_value() const = 0;

protected:
    base_ptree_item(name_type const& name, type_type const& type)
        : m_name(name)
        , m_type(type)
    {}

    static void from_ptree_info(ptree_type const& tree, name_type& name, type_type& type)
    {
        auto iter = tree.find(to_utf8_string(u8"name"));
        if (iter == tree.not_found())
        {
            KIWI_FAST_THROW_DESCR(logic_error, u8"没有找到 name");
        }
        name = from_utf8_string<char8_t>(iter->second.data());

        iter = tree.find(to_utf8_string(u8"type"));
        if (iter == tree.not_found())
        {
            KIWI_FAST_THROW_DESCR(logic_error, u8"没有找到 type");
        }
        type = from_utf8_string<char8_t>(iter->second.data());
    }

    static type_type set_v_type()
    {
        return u8"ptree_item_set";
    }

private:
    name_type m_name;
    type_type m_type;
};

template<bool IsSet>
class basic_ptree_item : public base_ptree_item
{
public:
    using value_type = typename base_ptree_item::item_value_type;

    //
    basic_ptree_item(name_type const& name, data_value const& value)
        :base_ptree_item(name, *value.inside_type())
        , m_value(value)
    {}

    basic_ptree_item(name_type const& name, data_value&& value)
        :base_ptree_item(name, *value.inside_type())
        , m_value(std::move(value))
    {}

    ~basic_ptree_item()
    {}

    void add(std::shared_ptr<base_ptree_item> const& ptree_item_) override
    {
        //空
    }

    void remove(name_type const& name_) override
    {
        //空
    }

    ptree_type to_ptree() const override
    {
        ptree_type item_tree;
        item_tree.put(to_utf8_string(u8"name"), to_utf8_string(name_type(this->name())));
        item_tree.put(to_utf8_string(u8"type"), to_utf8_string(type_type(this->v_type())));
		service<service_data_value> data_value_service;
		auto str = data_value_service->data_value_to_string(m_value);
		if (str)
		{
			item_tree.put(to_utf8_string(u8"value"), to_utf8_string(*str));
		}

        return item_tree;
    }

    ptree_type to_root_ptree() const override
    {
        ptree_type kiwi_fast_tree;

        ptree_type root_tree;
        root_tree.put_child(to_utf8_string(u8"kiwi_fast"), kiwi_fast_tree);

        return root_tree;
    }

    std::optional<std::reference_wrapper<item_value_type>> item_value()
    {
        return std::ref(m_value);
    }

    std::optional<std::reference_wrapper<const item_value_type>> item_value() const
    {
        return std::cref(m_value);
    }

    std::optional<std::reference_wrapper<item_set_value_type>> item_set_value() override
    {
        return {};
    }

    std::optional<std::reference_wrapper<const item_set_value_type>> item_set_value() const override
    {
        return {};
    }

private:
    value_type m_value;
};

template<>
class basic_ptree_item<true> : public base_ptree_item
{
public:
    using value_type = typename base_ptree_item::item_set_value_type;

    //
    basic_ptree_item(name_type const& name)
        :base_ptree_item(name, base_ptree_item::set_v_type())
    {}

    ~basic_ptree_item()
    {}

    void add(std::shared_ptr<base_ptree_item> const& ptree_item_) override
    {
        m_value.push_back(ptree_item_);
    }

    void remove(name_type const& name_) override
    {
        m_value.erase(std::remove_if(m_value.begin(), m_value.end(), [&](auto const& element) {
            if (name_type(element->name()) == name_)
            {
                return true;
            }
            else
            {
                return false;
            }
            }), m_value.end());
    }

    ptree_type to_ptree() const override
    {
        ptree_type item_tree;
        item_tree.put(to_utf8_string(u8"name"), to_utf8_string(name_type(this->name())));
        item_tree.put(to_utf8_string(u8"type"), to_utf8_string(type_type(this->v_type())));

		ptree_type sub_item_tree;
		std::for_each(m_value.begin(), m_value.end(), [&](auto const& ptree_item_) {
			sub_item_tree.add_child(to_utf8_string(u8"item"), ptree_item_->to_ptree());
			});

		item_tree.put_child(to_utf8_string(u8"value"), sub_item_tree);

        return item_tree;
    }

    ptree_type to_root_ptree() const override
    {
        ptree_type kiwi_fast_tree;

        std::for_each(m_value.begin(), m_value.end(), [&](auto const& ptree_item_) {
            kiwi_fast_tree.add_child(to_utf8_string(u8"item"), ptree_item_->to_ptree());
            });

        ptree_type root_tree;
        root_tree.put_child(to_utf8_string(u8"kiwi_fast"), kiwi_fast_tree);

        return root_tree;
    }

    std::optional<std::reference_wrapper<item_value_type>> item_value()
    {
        return {};
    }

    std::optional<std::reference_wrapper<const item_value_type>> item_value() const
    {
        return {};
    }

    std::optional<std::reference_wrapper<item_set_value_type>> item_set_value() override
    {
        return m_value;
    }

    std::optional<std::reference_wrapper<const item_set_value_type>> item_set_value() const override
    {
        return m_value;
    }

private:
    value_type m_value;
};

using ptree_item = basic_ptree_item<false>;
using ptree_item_set = basic_ptree_item<true>;
using ptree_root = basic_ptree_item<true>;

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
