#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <string>
#include <deque>
#include <filesystem>
#include <any>

//类型与字符串之间的转换

//因为值与字符串转换中使用deque，所以类型与字符串转换也使用deque

//注意对于不同的编译器，相同类型产生的字符串会不一样，所以只能手工指定

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

namespace type_converter
{
    template<typename T, typename CharType = char8_t>
    inline std::basic_string<CharType> to_string();

    template<typename T, typename CharType = char8_t>
    inline std::basic_string<CharType> to_string(T const*)
    {
        return to_string<T>();
    }
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define TYPE_CONVERTER_TO_U8STRING(type__, u8string__)                                       \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                  \
    namespace type_converter                                                                 \
    {                                                                                        \
        template<>                                                                           \
        inline std::basic_string<char8_t> to_string<type__, char8_t>()                       \
        {                                                                                    \
            return u8string__;                                                               \
        }                                                                                    \
        template<>                                                                           \
        inline std::basic_string<char8_t> to_string<type__*, char8_t>()                      \
        {                                                                                    \
            return to_string<type__, char8_t>() + u8"*";                                     \
        }                                                                                    \
        template<>                                                                           \
        inline std::basic_string<char8_t> to_string<std::deque<type__>, char8_t>()           \
        {                                                                                    \
            return std::u8string(u8"std::deque<") + to_string<type__, char8_t>() + u8">";    \
        }                                                                                    \
        template<>                                                                           \
        inline std::basic_string<char8_t> to_string<std::deque<type__*>, char8_t>()          \
        {                                                                                    \
            return std::u8string(u8"std::deque<") + to_string<type__*, char8_t>() + u8">";   \
        }                                                                                    \
    }                                                                                        \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define TYPE_CONVERTER_TO_U8STRING_NO_DEQUE(type__, u8string__)                              \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                  \
    namespace type_converter                                                                 \
    {                                                                                        \
        template<>                                                                           \
        inline std::basic_string<char8_t> to_string<type__, char8_t>()                       \
        {                                                                                    \
            return u8string__;                                                               \
        }                                                                                    \
        template<>                                                                           \
        inline std::basic_string<char8_t> to_string<type__*, char8_t>()                      \
        {                                                                                    \
            return to_string<type__, char8_t>() + u8"*";                                     \
        }                                                                                    \
        template<>                                                                           \
        inline std::basic_string<char8_t> to_string<std::deque<type__*>, char8_t>()          \
        {                                                                                    \
            return std::u8string(u8"std::deque<") + to_string<type__*, char8_t>() + u8">";   \
        }                                                                                    \
    }                                                                                        \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

//////////////

TYPE_CONVERTER_TO_U8STRING(bool, u8"bool")

//注意下面是三种不同类型
TYPE_CONVERTER_TO_U8STRING(char, u8"char")
TYPE_CONVERTER_TO_U8STRING(unsigned char, u8"unsigned char")
TYPE_CONVERTER_TO_U8STRING(signed char, u8"signed char")

TYPE_CONVERTER_TO_U8STRING(wchar_t, u8"wchar_t")
TYPE_CONVERTER_TO_U8STRING(char8_t, u8"char8_t")

TYPE_CONVERTER_TO_U8STRING(std::string, u8"std::string")
TYPE_CONVERTER_TO_U8STRING(std::wstring, u8"std::wstring")
TYPE_CONVERTER_TO_U8STRING(std::u8string, u8"std::u8string")

TYPE_CONVERTER_TO_U8STRING(short, u8"short")
TYPE_CONVERTER_TO_U8STRING(unsigned short, u8"unsigned short")

TYPE_CONVERTER_TO_U8STRING(int, u8"int")
TYPE_CONVERTER_TO_U8STRING(unsigned int, u8"unsigned int")

//long不适合跨平台，所以不提供

TYPE_CONVERTER_TO_U8STRING(long long, u8"long long")
TYPE_CONVERTER_TO_U8STRING(unsigned long long, u8"unsigned long long")

//注意 std::uint64_t std::int64_t等只是类型别名
//注意 std::size_t等只是类型别名

TYPE_CONVERTER_TO_U8STRING(float, u8"float")
TYPE_CONVERTER_TO_U8STRING(double, u8"double")

TYPE_CONVERTER_TO_U8STRING_NO_DEQUE(void, u8"void")

//注意 std::filesystem::path对应的是boost::filesystem::wpath

TYPE_CONVERTER_TO_U8STRING(std::filesystem::path, u8"std::filesystem::path")
TYPE_CONVERTER_TO_U8STRING(std::any, u8"std::any")

