#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>

#include <string>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class service_command_line
{
protected:
    virtual ~service_command_line(){}

public:

    /*!
     *
     */
    template<typename CharType>
    void add_option(CharType const* name, CharType const* description)
    {
        add_option<CharType>(std::basic_string<CharType>(name)
                             , std::basic_string<CharType>(description));
    }
    template<typename CharType>
    void add_option(std::basic_string<CharType> const& name, std::basic_string<CharType> const& description)
    {
        add_option(
                    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name)
                    , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description));
    }
    virtual void add_option(std::u8string const& name, std::u8string const& description) = 0;

    /*!
     *
     */
    template<typename CharType>
    void add_int_option(CharType const* name, CharType const* description)
    {
        add_int_option<CharType>(std::basic_string<CharType>(name)
                             , std::basic_string<CharType>(description));
    }
    template<typename CharType>
    void add_int_option(std::basic_string<CharType> const& name, std::basic_string<CharType> const& description)
    {
        add_int_option(
                    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name)
                    , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(description));
    }
    virtual void add_int_option(std::u8string const& name, std::u8string const& description) = 0;
};


KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
