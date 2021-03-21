#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/data_value.h>

#include <string>
#include <any>
#include <optional>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE
class service_data_value;
KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

TYPE_CONVERTER_TO_U8STRING_NO_DEQUE(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_data_value, u8"service_data_value")

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class service_data_value
{
protected:
    virtual ~service_data_value() {}

public:
    /*!
     *
     */
    template<typename CharType>
    data_value create_data_value(CharType const* class_name)
    {
        return create_data_value(std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    data_value create_data_value(std::basic_string<CharType> const& class_name)
    {
        return create_data_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual data_value create_data_value(const char8_t* class_name) = 0;

    /*!
     *
     */
    virtual std::optional<std::u8string> data_value_to_string(data_value const& dv) = 0;


    /*!
     *
     */
    template<typename CharType>
    void data_value_from_string(CharType const* str, CharType const* class_name, data_value& dv)
    {
        data_value_from_string(std::basic_string<CharType>(str)
            , std::basic_string<CharType>(class_name)
            , dv);
    }
    template<typename CharType>
    void data_value_from_string(std::basic_string<CharType> const& str, std::basic_string<CharType> const& class_name, data_value& dv)
    {
        data_value_from_string(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(str).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str()
            , dv);
    }
    virtual void data_value_from_string(const char8_t* str, const char8_t* class_name, data_value& dv) = 0;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE