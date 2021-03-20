#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

#include <kiwi.fast.plugin_utility/resource_object_factory.h>

#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/service_object_factory.h>

#include <boost/property_tree/ptree.hpp>

#include <string>
#include <any>
#include <variant>
#include <deque>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

/*!
 * \brief The ptree_item struct
 */
class ptree_item
{
    friend class ptree_root;
public:
    //使用ptree，而不是wptree，是因为wptree写入读取文件时使用wiofstream，他在mingw下无法正常工作(解决方式麻烦)
    using ptree_type = boost::property_tree::ptree;

    using name_type = std::u8string;
    using type_type = std::u8string;
    using value_type = std::any;

    template<typename T>
    using resource_object_factory_value_type = resource_object_factory<T>;

    template<typename T>
    using resource_deque_factory_value_type = resource_deque_factory<T>;

    //
    ptree_item()
    {}


    //

    template<typename T>
    ptree_item(name_type const& name, resource_object_factory_value_type<T> const& value)
        :m_name(name)
        , m_type(type_converter::to_string<T>())
        , m_value(value)
    {}

    template<typename T>
    ptree_item(name_type const& name, resource_deque_factory_value_type<T> const& value)
        :m_name(name)
        ,m_type(type_converter::to_string<T>())
        ,m_value(value)
    {}

    ptree_item(name_type const& name, type_type const& type, value_type const& value)
        :m_name(name)
        , m_type(type)
        , m_value(value)
    {}

    //
    ptree_item(name_type const& name_);

    ~ptree_item();

    void add(ptree_item const& ptree_item_);

    void remove(name_type const& name_);

    ptree_type to_ptree() const;

    static ptree_item from_ptree(ptree_type const& tree);

    name_type& name()
    {
        return m_name;
    }

    name_type const& name() const
    {
        return m_name;
    }

    type_type& type()
    {
        return m_type;
    }

    type_type const& type() const
    {
        return m_type;
    }

    value_type& value()
    {
        return m_value;
    }

    value_type const& value() const
    {
        return m_value;
    }

private:
    name_type m_name;
    type_type m_type;

    //m_value中存放的是 resource_object_factory<T> 或者  resource_deque_factory<T> 或者 std::deque<ptree_item> 类型对象
    value_type m_value;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

TYPE_CONVERTER_TO_U8STRING(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_item, u8"ptree_item")
