#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

#include <string>
#include <any>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE
class service_object_factory;
KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

TYPE_CONVERTER_TO_U8STRING_NO_DEQUE(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory, u8"service_object_factory")

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
    std::any create_object(CharType const* class_name)
    {
        return create_object(std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    std::any create_object(std::basic_string<CharType> const& class_name)
    {
        return create_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual std::any create_object(const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    void destroy_object(std::any& object, CharType const* class_name)
    {
        destroy_object(object, std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    void destroy_object(std::any& object, std::basic_string<CharType> const& class_name)
    {
        destroy_object(object, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual void destroy_object(std::any& object, const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    std::any create_object_deque(CharType const* class_name)
    {
        return create_object_deque(std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    std::any create_object_deque(std::basic_string<CharType> const& class_name)
    {
        return create_object_deque(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual std::any create_object_deque(const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    void destroy_object_deque(std::any& deque, CharType const* class_name)
    {
        destroy_object_deque(deque, std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    void destroy_object_deque(std::any& deque, std::basic_string<CharType> const& class_name)
    {
        destroy_object_deque(deque, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual void destroy_object_deque(std::any& deque, const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    std::u8string object_to_string(std::any const& object, CharType const* class_name)
    {
        return object_to_string(object, std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    std::u8string object_to_string(std::any const& object, std::basic_string<CharType> const& class_name)
    {
        return object_to_string(object, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual std::u8string object_to_string(std::any const& object, const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    void string_to_object(CharType const* str, std::any& object, CharType const* class_name)
    {
        return string_to_object(std::basic_string<CharType>(str), object
                                , std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    void string_to_object(std::basic_string<CharType> const& str, std::any& object, std::basic_string<CharType> const& class_name)
    {
        return string_to_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(str).c_str(), object
                                , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual void string_to_object(const char8_t* str, std::any& object, const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    std::u8string deque_to_string(std::any const& object, CharType const* class_name)
    {
        return deque_to_string(object, std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    std::u8string deque_to_string(std::any const& object, std::basic_string<CharType> const& class_name)
    {
        return deque_to_string(object, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual std::u8string deque_to_string(std::any const& object, const char8_t* class_name) = 0;

    /*!
     *
     */
    template<typename CharType>
    void string_to_deque(CharType const* str, std::any& object, CharType const* class_name)
    {
        string_to_deque(std::basic_string<CharType>(str), object, std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    void string_to_deque(std::basic_string<CharType> const& str, std::any& object, std::basic_string<CharType> const& class_name)
    {
        string_to_deque(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(str).c_str()
                               , object
                               , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual void string_to_deque(const char8_t* str, std::any& object, const char8_t* class_name) = 0;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
