#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
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
    friend class setting_visitor;
    friend class ptree_visitor;
    friend class ptree_root;

public:
    //使用ptree，而不是wptree，是因为wptree写入读取文件时使用wiofstream，他在mingw下无法正常工作(解决方式麻烦)
    using ptree_type = boost::property_tree::ptree;

    using name_type = std::u8string;
    using type_type = std::u8string;
    using value_type = std::any;

    //
    ptree_item()
    {}

    //
    template<typename T>
    ptree_item(name_type const& name_, T* value_)
        :name(name_)
        , type(type_converter::to_string<T>())
        , value(value_)
    {}

    ptree_item(name_type const& name_, type_type const& type_, std::any const& value_)
        :name(name_)
        ,type(type_)
        ,value(value_)
    {}

    //
    ptree_item(name_type const& name_);

    ~ptree_item();

    void add(ptree_item const& ptree_item_);

    void remove(name_type const& name_);

    ptree_type to_ptree() const;

    static ptree_item from_ptree(ptree_type const& tree);

private:
    name_type name;
    type_type type;
    //std::any中存放的是T*或者std::deque<ptree_item>
    value_type value;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

TYPE_CONVERTER_TO_U8STRING(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_item, u8"ptree_item")
