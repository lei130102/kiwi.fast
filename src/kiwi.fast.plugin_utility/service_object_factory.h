#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>

#include <string>
#include <any>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class service_object_factory
{
protected:
    virtual ~service_object_factory(){}

public:
    /*!
     *
     */
    template<typename CharType>
    void* create_object(CharType const* class_name)
    {
        return create_object(std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    void* create_object(std::basic_string<CharType> const& class_name)
    {
        return create_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual void* create_object(const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    void destroy_object(void* object, CharType const* class_name)
    {
        destroy_object(object, std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    void destroy_object(void* object, std::basic_string<CharType> const& class_name)
    {
        destroy_object(object, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual void destroy_object(void* object, const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    void* create_object_deque(CharType const* class_name)
    {
        return create_object_deque(std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    void* create_object_deque(std::basic_string<CharType> const& class_name)
    {
        return create_object_deque(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual void* create_object_deque(const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    void destroy_object_deque(void* deque, CharType const* class_name)
    {
        destroy_object_deque(deque, std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    void destroy_object_deque(void* deque, std::basic_string<char8_t> const& class_name)
    {
        destroy_object_deque(deque, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual void destroy_object_deque(void* deque, const char8_t* class_name) = 0;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
