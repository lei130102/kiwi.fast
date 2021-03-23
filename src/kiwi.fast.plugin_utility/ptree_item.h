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

class base_ptree_visitor;

class base_ptree_item : public std::enable_shared_from_this<base_ptree_item>
{
public:
    //使用ptree，而不是wptree，是因为wptree写入读取文件时使用wiofstream，他在mingw下无法正常工作(解决方式麻烦)
    using ptree_type = boost::property_tree::ptree;

    using name_type = std::u8string;
    using type_type = std::u8string;
    using item_value_type = data_value;
    using item_set_value_type = std::deque<std::shared_ptr<base_ptree_item>>;

    virtual ~base_ptree_item();

    virtual void insert_front(const char8_t* name, std::shared_ptr<base_ptree_item> const& ptree_item_) = 0;

    virtual void insert_back(const char8_t* name, std::shared_ptr<base_ptree_item> const& ptree_item_) = 0;

    virtual void add(std::shared_ptr<base_ptree_item> const& ptree_item_) = 0;

    virtual void remove(name_type const& name) = 0;

    virtual ptree_type to_ptree() const = 0;

    virtual ptree_type to_root_ptree() const = 0;

    virtual type_type v_type() const = 0;

    template<typename CharType>
    base_ptree_visitor* item(CharType const* name)
    {
        return item(std::basic_string<CharType>(name));
    }
    template<typename CharType>
    base_ptree_visitor* item(std::basic_string<CharType> const& name)
    {
        return item(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str());
    }
    base_ptree_visitor* item(const char8_t* name);

    std::reference_wrapper<name_type> name()
    {
        return m_name;
    }

    std::reference_wrapper<const name_type> name() const
    {
        return m_name;
    }

    virtual std::optional<std::reference_wrapper<item_value_type>> item_value() = 0;

    virtual std::optional<std::reference_wrapper<const item_value_type>> item_value() const = 0;

    virtual std::optional<std::reference_wrapper<item_set_value_type>> item_set_value() = 0;

    virtual std::optional<std::reference_wrapper<const item_set_value_type>> item_set_value() const = 0;

protected:
    base_ptree_item(name_type const& name)
        : m_name(name),
        m_visitor(nullptr)
    {}

private:
    name_type m_name;

    base_ptree_visitor* m_visitor;
};

std::shared_ptr<base_ptree_item> create_ptree_item(base_ptree_item::name_type const& name, data_value const& value);
std::shared_ptr<base_ptree_item> create_ptree_item(base_ptree_item::name_type const& name, data_value&& value);
std::shared_ptr<base_ptree_item> create_ptree_item_set(base_ptree_item::name_type const& name);
std::shared_ptr<base_ptree_item> create_ptree_root();

template<bool IsSet>
class basic_ptree_item : public base_ptree_item
{
public:
    using value_type = typename base_ptree_item::item_value_type;

    friend std::shared_ptr<base_ptree_item> create_ptree_item(name_type const& name, data_value const& value);
    friend std::shared_ptr<base_ptree_item> create_ptree_item(name_type const& name, data_value&& value);
    
    void insert_front(const char8_t* name, std::shared_ptr<base_ptree_item> const& ptree_item_)
    {
        //空
    }

    void insert_back(const char8_t* name, std::shared_ptr<base_ptree_item> const& ptree_item_)
    {
        //空
    }

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

    type_type v_type() const override
    {
        return *m_value.inside_type();
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

protected:
    //
    basic_ptree_item(name_type const& name, data_value const& value)
        :base_ptree_item(name)
        , m_value(value)
    {}

    basic_ptree_item(name_type const& name, data_value&& value)
        :base_ptree_item(name)
        , m_value(std::move(value))
    {}

private:
    value_type m_value;
};


template<>
class basic_ptree_item<true> : public base_ptree_item
{
public:
    using value_type = typename base_ptree_item::item_set_value_type;

    friend std::shared_ptr<base_ptree_item> create_ptree_item_set(name_type const& name);
    friend std::shared_ptr<base_ptree_item> create_ptree_root();

    void insert_front(const char8_t* name, std::shared_ptr<base_ptree_item> const& ptree_item_)
    {
        auto iter = std::find_if(m_value.begin(), m_value.end(), [&](auto const& element) {
            if (name_type(element->name()) == name)
            {
                return true;
            }
            else
            {
                return false;
            }
        });
        if (iter != m_value.end())
        {
            m_value.insert(iter, ptree_item_);
        }
        else
        {
            KIWI_FAST_THROW_DESCR(logic_error, std::u8string(u8"插入失败，没有找到名称，名称：") + name);
        }
    }

    void insert_back(const char8_t* name, std::shared_ptr<base_ptree_item> const& ptree_item_)
    {
        auto iter = std::find_if(m_value.begin(), m_value.end(), [&](auto const& element) {
            if (name_type(element->name()) == name)
            {
                return true;
            }
            else
            {
                return false;
            }
        });
        if (iter != m_value.end())
        {
            m_value.insert(iter + 1, ptree_item_);
        }
        else
        {
            KIWI_FAST_THROW_DESCR(logic_error, std::u8string(u8"插入失败，没有找到名称，名称：") + name);
        }
    }

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

    static type_type item_set_type()
    {
        return u8"ptree_item_set";
    }

    type_type v_type() const override
    {
        return basic_ptree_item::item_set_type();
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

protected:
    basic_ptree_item(name_type const& name)
        :base_ptree_item(name)
    {}

private:
    value_type m_value;
};

using ptree_item = basic_ptree_item<false>;
using ptree_item_set = basic_ptree_item<true>;
using ptree_root = basic_ptree_item<true>;

inline std::shared_ptr<base_ptree_item> create_ptree_item(base_ptree_item::name_type const& name, data_value const& value)
{
	return std::dynamic_pointer_cast<base_ptree_item>(std::shared_ptr<ptree_item>(new ptree_item(name, value)));
}

inline std::shared_ptr<base_ptree_item> create_ptree_item(base_ptree_item::name_type const& name, data_value&& value)
{
	return std::dynamic_pointer_cast<base_ptree_item>(std::shared_ptr<ptree_item>(new ptree_item(name, std::move(value))));
}

inline std::shared_ptr<base_ptree_item> create_ptree_item_set(base_ptree_item::name_type const& name)
{
	return std::dynamic_pointer_cast<base_ptree_item>(std::shared_ptr<ptree_item_set>(new ptree_item_set(name)));
}

inline std::shared_ptr<base_ptree_item> create_ptree_root()
{
	return std::dynamic_pointer_cast<base_ptree_item>(std::shared_ptr<ptree_root>(new ptree_root(u8"")));
}

namespace
{
	inline void get_info_by_ptree(base_ptree_item::ptree_type const& tree
		, base_ptree_item::name_type& name
		, base_ptree_item::type_type& type)
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
}

inline std::shared_ptr<base_ptree_item> create_basic_ptree_item_by_ptree(base_ptree_item::ptree_type const& tree)
{
    base_ptree_item::name_type name;
    base_ptree_item::type_type type;
    get_info_by_ptree(tree, name, type);
    if (type != ptree_item_set::item_set_type())
    {
        auto iter = tree.find(to_utf8_string(u8"value"));
        std::u8string value = from_utf8_string<char8_t>(iter->second.data());

        return create_ptree_item(name, data_value::from_string(value, type));
    }
    else if (type == ptree_item_set::item_set_type())
    {
        auto iter = tree.find(to_utf8_string(u8"value"));

        std::shared_ptr<base_ptree_item> item(create_ptree_item_set(name));
        for (auto const& child_tree : iter->second)
        {
            if (child_tree.first == to_utf8_string(u8"item"))
            {
                std::shared_ptr<base_ptree_item> sub_item = create_basic_ptree_item_by_ptree(child_tree.second);
                item->add(sub_item);
            }
        }
        return item;
    }
    else
    {
        return std::shared_ptr<base_ptree_item>();
    }
}

inline std::shared_ptr<base_ptree_item> create_ptree_root_by_ptree(base_ptree_item::ptree_type const& tree)
{
    std::shared_ptr<base_ptree_item> root(create_ptree_root());

    base_ptree_item::ptree_type kiwi_fast_tree = tree.get_child(to_utf8_string(u8"kiwi_fast"));

    for (auto const& child_tree : kiwi_fast_tree)
    {
        if (child_tree.first == to_utf8_string(u8"item"))
        {
            std::shared_ptr<base_ptree_item> item = create_basic_ptree_item_by_ptree(child_tree.second);
            root->add(item);
        }
    }

    return root;
}

namespace detail
{
    inline std::optional<std::shared_ptr<base_ptree_item>> find_ptree_item_by_names(base_ptree_item* ptree_root_, std::deque<std::u8string>& names)
    {
        if (names.size() == 0)
        {
            return {};
        }

        if (ptree_root_ == nullptr)
        {
            return {};
        }

        if (!ptree_root_->item_set_value())
        {
            return {};
        }

        for (auto& basic_ptree_item_ : (*ptree_root_->item_set_value()).get())
        {
            if (basic_ptree_item_->name().get() == names.front())
            {
                if (basic_ptree_item_->v_type() == ptree_item_set::item_set_type())
                {
                    names.pop_front();
                    return find_ptree_item_by_names(basic_ptree_item_.get(), names);
                }
                else
                {
                    return basic_ptree_item_;
                }
            }
        }
        return {};
    }
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
