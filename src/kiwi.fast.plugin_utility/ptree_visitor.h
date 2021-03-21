#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/ptree_item.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

#include <string>
#include <deque>
#include <optional>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE
//////
////////class ptree_root;
//////
//////class ptree_visitor
//////{
//////    //friend class ptree_root;
//////
//////public:
//////    using name_type = base_ptree_item::name_type;
//////    using type_type = base_ptree_item::type_type;
//////    //using value_type = ptree_item::value_type;
//////
//////    ptree_visitor()
//////        :m_ptree_root(nullptr)
//////    {}
//////
//////    ptree_visitor(ptree_root* ptree_root_)
//////        :m_ptree_root(ptree_root_)
//////    {}
//////
//////    virtual ~ptree_visitor()
//////    {}
//////
//////    ptree_visitor& operator()(const char8_t* name)
//////    {
//////        add_name(name);
//////        return *this;
//////    }
//////
//////    virtual std::optional<name_type> name()
//////    {
//////        auto ptree_item = find_object_value_item(m_names);
//////        if(ptree_item)
//////        {
//////            return (*ptree_item).get().name();
//////        }
//////        else
//////        {
//////            return {};
//////        }
//////    }
//////
//////    virtual std::optional<type_type> type()
//////    {
//////        auto ptree_item = find_object_value_item(m_names);
//////        if(ptree_item)
//////        {
//////            return (*ptree_item).get().type();
//////        }
//////        else
//////        {
//////            return {};
//////        }
//////    }
//////
//////    std::optional<data_value> value()
//////    {
//////        auto result = original_value();
//////        if (result)
//////        {
//////            try
//////            {
//////                return std::get<data_value>(*result);
//////            }
//////            catch (std::bad_variant_access& e)
//////            {
//////                return {};
//////            }
//////        }
//////        else
//////        {
//////            return {};
//////        }
//////    }
//////
//////    virtual std::optional<value_type> original_value()
//////    {
//////        auto ptree_item = find_object_value_item(m_names);
//////        if (ptree_item)
//////        {
//////            return (*ptree_item).get().value();
//////        }
//////        else
//////        {
//////            return {};
//////        }
//////    }
//////
//////    bool set_value(data_value&& dv)
//////    {
//////        if (!dv.is_valid())
//////        {
//////            KIWI_FAST_THROW_DESCR(logic_error, u8"data_value对象不是有效的");
//////            return false;
//////        }
//////        auto ptree_item = find_object_value_item(m_names);
//////        if(ptree_item)
//////        {
//////            //设置type和value
//////            (*ptree_item).get().type() = *dv.inside_type();
//////            (*ptree_item).get().value() = dv;
//////
//////            return true;
//////        }
//////        else
//////        {
//////            return false;
//////        }
//////    }
//////
//////protected:
//////
//////    void add_name(std::u8string const& name)
//////    {
//////        m_names.push_back(name);
//////    }
//////
//////    std::optional<std::reference_wrapper<ptree_item>> find_object_value_item(std::deque<std::u8string>& names);
//////
//////private:
//////    std::deque<std::u8string> m_names;
//////    std::shared_ptr<base_ptree_item> m_ptree_root;
//////};
//////
//////
//////
//////template<typename CharType>
//////ptree_visitor item(CharType const* name)
//////{
//////    return item(std::basic_string<CharType>(name));
//////}
//////template<typename CharType>
//////ptree_visitor item(std::basic_string<CharType> const& name)
//////{
//////    return item(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str());
//////}
//////
//////ptree_visitor item(const char8_t* name)
//////{
//////    ptree_visitor visitor(shared_from_this());
//////    visitor.add_name(name);
//////    return visitor;
//////}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
