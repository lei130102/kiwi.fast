#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>

#include <string>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class manager_external_interface
{
protected:
    virtual ~manager_external_interface()
    {}

public:
    /*!
     *
     */
    template<typename CharType>
    bool query(const CharType* name, void** address)
    {
        return query(std::basic_string<CharType>(name), address);
    }
    template<typename CharType>
    bool query(std::basic_string<CharType> const& name, void** address)
    {
        return query(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str(), address);
    }
    virtual bool query(const char8_t* name, void** address) = 0;

    /*!
     *
     */
    template<typename CharType>
    bool query_destroy(const CharType* name)
    {
        query_destroy(std::basic_string<CharType>(name));
    }
    template<typename CharType>
    void query_destroy(std::basic_string<CharType> const& name)
    {
        query_destroy(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str());
    }
    virtual void query_destroy(const char8_t* name) = 0;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
