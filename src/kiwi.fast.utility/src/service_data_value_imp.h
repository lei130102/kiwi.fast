#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/data_value.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <functional>
#include <map>
#include <deque>
#include <any>
#include <optional>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_data_value_imp
{
public:

    //

    using create_data_value_function_type = std::function<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value()>;

    using u8string_create_data_value_function_map_type = std::map<std::u8string, create_data_value_function_type>;

    //

    using data_value_to_u8string_function_type = std::function<std::optional<std::u8string>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value const&)>;
    using data_value_from_u8string_function_type = std::function<void(const char8_t* str, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value&)>;

    using data_value_to_u8string_function_map_type = std::map<std::u8string, data_value_to_u8string_function_type>;
    using data_value_from_u8string_function_map_type = std::map<std::u8string, data_value_from_u8string_function_type>;

public:

    template<typename FunctionType, typename ObjectMap, typename DequeMap>
    FunctionType find_function_by_class_name(const char8_t* class_name, ObjectMap object_map, DequeMap deque_map)
    {
        std::u8string class_name_ = class_name;
        if (parse_class_name(class_name, class_name_))
        {
            auto iter = deque_map.find(class_name_);
            if (iter != deque_map.end())
            {
                return (*iter).second;
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法根据类型名找到对应函数，类型名：") + class_name_);
                return FunctionType{};
            }
        }
        else
        {
            auto iter = object_map.find(class_name_);
            if (iter != object_map.end())
            {
                return (*iter).second;
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法根据类型名找到对应函数，类型名：") + class_name_);
                return FunctionType{};
            }
        }
    }

    virtual KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value create_data_value(const char8_t* class_name)
    {
        create_data_value_function_type f = find_function_by_class_name<create_data_value_function_type>
            (class_name, m_create_data_object_value_map, m_create_data_deque_value_map);
        if (f)
        {
            return f();
        }
        else
        {
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value{};
        }
    }

    virtual std::optional<std::u8string> data_value_to_string(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value const& dv)
    {
        data_value_to_u8string_function_type f = find_function_by_class_name<data_value_to_u8string_function_type>
            ((*dv.inside_type()).c_str(), m_data_value_object_to_u8string_map, m_data_value_deque_to_u8string_map);
        if (f)
        {
            return f(dv);
        }
        else
        {
            return {};
        }
    }

    virtual void data_value_from_string(const char8_t* str, const char8_t* class_name, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value& dv)
    {
        data_value_from_u8string_function_type f = find_function_by_class_name<data_value_from_u8string_function_type>
            (class_name, m_data_value_object_from_u8string_map, m_data_value_deque_from_u8string_map);
        if (f)
        {
            f(str, dv);
        }
    }

protected:
    template<typename T, typename TImp = T>
    void insert_object_create_destroy()
    {
        service_data_value_imp::m_create_data_object_value_map.insert(u8string_create_data_value_function_map_type::value_type(
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
            , []() {return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<T>()); }
        ));
    };
    template<typename T, typename TImp = T>
    void insert_deque_create_destroy()
    {
        service_data_value_imp::m_create_data_deque_value_map.insert(u8string_create_data_value_function_map_type::value_type(
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
            , []() {return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_deque_value<T>()); }
        ));
    };

    template<typename T, typename FToString, typename FFromString>
    void insert_object_u8string(FToString f_to_string, FFromString f_from_string)
    {
        service_data_value_imp::m_data_value_object_to_u8string_map.insert(data_value_to_u8string_function_map_type::value_type(
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
            ,
            [f = f_to_string](KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value const& dv) { return f(dv.pointer<T>()); }
        ));

        service_data_value_imp::m_data_value_object_from_u8string_map.insert(data_value_from_u8string_function_map_type::value_type(
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
            ,
            [f = f_from_string](std::u8string const& str, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value& dv) {f(str, dv.pointer<T>()); }
        ));
    }

    template<typename T, typename FToString, typename FFromString>
    void insert_deque_u8string(FToString f_to_string, FFromString f_from_string)
    {
        service_data_value_imp::m_data_value_deque_to_u8string_map.insert(data_value_to_u8string_function_map_type::value_type(
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
            ,
            [f = f_to_string](KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value const& dv) { return f(dv.pointer<std::deque<T*>>()); }
        ));
        service_data_value_imp::m_data_value_deque_from_u8string_map.insert(data_value_from_u8string_function_map_type::value_type(
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
            ,
            [f = f_from_string](std::u8string const& str, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value& dv) {f(str, dv.pointer<std::deque<T*>>()); }
        ));
    }

    service_data_value_imp();

    virtual ~service_data_value_imp()
    {}

    bool parse_class_name(const char8_t* class_name, std::u8string& deque_element_class_name)
    {
        static std::u8string const deque_prefix = u8"std::deque<";
        static std::size_t const deque_prefix_length = deque_prefix.length();

        std::u8string class_name_ = class_name;
        std::size_t class_name_length = class_name_.length();
        if ((class_name_.substr(0, deque_prefix_length) == deque_prefix)
            && (class_name_[class_name_length - 1] == u8'>'))
        {
            //比如class_name是std::deque<int*>，那么deque_element_class_name就是int
            deque_element_class_name = class_name_.substr(deque_prefix_length, class_name_length - deque_prefix_length - 1 - 1);
            return true;
        }
        return false;
    }

private:
    static u8string_create_data_value_function_map_type m_create_data_object_value_map;

    static u8string_create_data_value_function_map_type m_create_data_deque_value_map;

    //

    static data_value_to_u8string_function_map_type m_data_value_object_to_u8string_map;
    static data_value_from_u8string_function_map_type m_data_value_object_from_u8string_map;

    static data_value_to_u8string_function_map_type m_data_value_deque_to_u8string_map;
    static data_value_from_u8string_function_map_type m_data_value_deque_from_u8string_map;
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE

#define SERVICE_DATA_VALUE_ADAPTER_METHOD(imp_class)                                                                                                       \
    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value create_data_value(const char8_t* class_name)                                                   \
    {                                                                                                                                                      \
        return imp_class::create_data_value(class_name);                                                                                                   \
    }                                                                                                                                                      \
    std::optional<std::u8string> data_value_to_string(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value const& dv)                                   \
    {                                                                                                                                                      \
        return imp_class::data_value_to_string(dv);                                                                                                        \
    }                                                                                                                                                      \
    void data_value_from_string(const char8_t* str, const char8_t* class_name, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value& dv)                \
    {                                                                                                                                                      \
        imp_class::data_value_from_string(str, class_name, dv);                                                                                            \
    }