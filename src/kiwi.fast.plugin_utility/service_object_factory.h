#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

#include <string>
#include <any>
#include <optional>

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


    template<typename CharType>
    std::any copy_object(CharType const* class_name, std::any const& rhs)
    {
        return copy_object(std::basic_string<CharType>(class_name), rhs);
    }
    template<typename CharType>
    std::any copy_object(std::basic_string<CharType> const& class_name, std::any const& rhs)
    {
        return copy_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str(), rhs);
    }
    virtual std::any copy_object(const char8_t* class_name, std::any const& rhs) = 0;

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
    std::optional<std::u8string> object_to_string(std::any const& object, CharType const* class_name)
    {
        return object_to_string(object, std::basic_string<CharType>(class_name));
    }
    template<typename CharType>
    std::optional<std::u8string> object_to_string(std::any const& object, std::basic_string<CharType> const& class_name)
    {
        return object_to_string(object, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
    }
    virtual std::optional<std::u8string> object_to_string(std::any const& object, const char8_t* class_name) = 0;




    /*!
     * 注意，要求object为空(即初始std::any对象)
     */
    template<typename CharType>
    void object_from_string(CharType const* str, CharType const* class_name, std::any& object)
    {
        object_from_string(std::basic_string<CharType>(str)
            , std::basic_string<CharType>(class_name)
            , object);
    }
    template<typename CharType>
    void object_from_string(std::basic_string<CharType> const& str, std::basic_string<CharType> const& class_name, std::any& object)
    {
        object_from_string(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(str).c_str()
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str()
            , object);
    }
    virtual void object_from_string(const char8_t* str, const char8_t* class_name, std::any& object) = 0;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
