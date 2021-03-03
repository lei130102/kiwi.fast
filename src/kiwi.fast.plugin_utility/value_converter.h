#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>

#include <deque>
#include <string>
#include <sstream>
#include <numeric>

//类型值与字符串之间的转换

//因为vector<bool>比较特殊，所以为了统一，都用deque

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

namespace value_converter
{
    //xml格式
    struct xml{};
    //json格式
    struct json{};
    //ini格式
    struct ini{};
    //显示格式
    struct display{};

    template<typename Format, typename T, typename CharType = char8_t>
    inline std::basic_string<CharType> to_string(T const& value, bool* is_error = nullptr);
    template<typename Format, typename T, typename CharType = char8_t>
    inline std::basic_string<CharType> to_string(T* pointer, bool* is_error = nullptr);
    template<typename Format, typename T, typename CharType = char8_t>
    inline T from_string(std::basic_string<CharType> const& str, bool* is_error = nullptr);

    template<typename Format, typename T, typename CharType = char8_t>
    inline std::basic_string<CharType> deque_to_string(std::deque<T> const& deq, std::basic_string<CharType> const& sep, bool* is_error = nullptr)
    {
        bool is_error_ = false;
        if(is_error != nullptr)
        {
            *is_error = false;
        }

        std::basic_string<CharType> result;
        for(auto iter = deq.begin(); iter != deq.end(); ++iter)
        {
            std::basic_string<CharType> str = to_string<Format, T, CharType>(*iter, &is_error_);

            if(is_error_)
            {
                if(is_error != nullptr)
                {
                    *is_error = true;
                }
                return std::basic_string<CharType>();
            }

            result += str;
            if((iter + 1) != deq.end())
            {
                result += sep;
            }
        }
        return result;
    }

    template<typename Format, typename T, typename CharType = char8_t>
    inline std::basic_string<CharType> deque_to_string(std::deque<T*> const& deq, std::basic_string<CharType> const& sep, bool* is_error = nullptr)
    {
        bool is_error_ = false;
        if(is_error != nullptr)
        {
            *is_error = false;
        }

        std::basic_string<CharType> result;
        for(auto const& iter = deq.begin(); iter != deq.end(); ++iter)
        {
            std::basic_string<CharType> str = to_string<Format, T, CharType>(*iter, &is_error_);

            if(is_error_)
            {
                if(is_error != nullptr)
                {
                    *is_error = true;
                }
                return std::basic_string<CharType>();
            }

            result += str;
            if((iter + 1) != deq.end())
            {
                result += sep;
            }
        }
        return result;
    }

    template<typename Format, typename T, typename CharType = char8_t>
    inline std::deque<T> string_to_deque(std::basic_string<CharType> const& str, std::basic_string<CharType> const& sep, bool* is_error = nullptr)
    {
        bool is_error_ = false;
        if(is_error != nullptr)
        {
            *is_error = false;
        }

        std::deque<T> result;

        if(str.empty())
        {
            return result;
        }

        bool parsing = true;
        typename std::basic_string<CharType>::size_type last_index = 0;
        while(parsing)
        {
            typename std::basic_string<CharType>::size_type index = str.find(sep, last_index);
            if(index == std::basic_string<CharType>::npos)
            {
                index = str.size();
            }
            std::basic_string<CharType> token = str.substr(last_index, (index - last_index));
            T parsed_value = from_string<Format, T, CharType>(token, &is_error_);
            if(is_error_)
            {
                if(is_error != nullptr)
                {
                    *is_error = true;
                }
                result.clear();
                return result;
            }
            result.push_back(parsed_value);
            last_index = index + sep.size();
            if(index == str.size())
            {
                parsing = false;
            }
        }
        return result;
    }
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

//BY_MEMBERFUNCTION和BY_STREAM两个宏不适合合并，因为输入参数不同

#define VALUE_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(format__, type__)                                               \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::basic_string<char8_t> to_string<format__, type__, char8_t>(type__ const& value, bool* is_error)   \
        {                                                                                                             \
            return value.to_##format__<char8_t>(is_error);                                                            \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define POINTER_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(format__, type__)                                             \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::basic_string<char8_t> to_string<format__, type__, char8_t>(type__* pointer, bool* is_error)       \
        {                                                                                                             \
            return pointer->to_##format__<char8_t>(is_error);                                                         \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define VALUE_CONVERTER_FROM_U8STRING_BY_MEMBERFUNCTION(format__, type__)                                             \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline type__ from_string<format__, type__, char8_t>(std::basic_string<CharType> const& str, bool* is_error)  \
        {                                                                                                             \
            type__ result;                                                                                            \
            result.from_##format__<char8_t>(is_error);                                                                \
            return result;                                                                                            \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION(format__, type__, precision__)                                \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::basic_string<char8_t> to_string<format__, type__, char8_t>(type__ const& value, bool* is_error)   \
        {                                                                                                             \
            std::stringstream buf;                                                                                    \
            if(precision__ != -1)                                                                                     \
            {                                                                                                         \
                buf.precision(precision__);                                                                           \
            }                                                                                                         \
            buf << value;                                                                                             \
            if(is_error != nullptr)                                                                                   \
            {                                                                                                         \
                *is_error = buf.fail();                                                                               \
            }                                                                                                         \
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(buf.str());                  \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define VALUE_CONVERTER_FROM_U8STRING_BY_STREAM(format__, type__)                                                     \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline type__ from_string<format__, type__, char8_t>(std::basic_string<char8_t> const& str, bool* is_error)   \
        {                                                                                                             \
            std::stringstream buf(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(str));           \
            type__ result;                                                                                            \
            buf >> result;                                                                                            \
            if(is_error != nullptr)                                                                                   \
            {                                                                                                         \
                *is_error = buf.fail();                                                                               \
            }                                                                                                         \
            return result;                                                                                            \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION_CAST(format__, type__, precision__, cast__)                   \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::basic_string<char8_t> to_string<format__, type__, char8_t>(type__ const& value, bool* is_error)   \
        {                                                                                                             \
            std::stringstream buf;                                                                                    \
            if(precision__ != -1)                                                                                     \
            {                                                                                                         \
                buf.precision(precision__);                                                                           \
            }                                                                                                         \
            buf << static_cast<cast__>(value);                                                                        \
            if(is_error != nullptr)                                                                                   \
            {                                                                                                         \
                *is_error = buf.fail();                                                                               \
            }                                                                                                         \
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(buf.str());                  \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define VALUE_CONVERTER_FROM_U8STRING_BY_STREAM_CAST(format__, type__, cast__)                                        \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline type__ from_string<format__, type__, char8_t>(std::basic_string<char8_t> const& str, bool* is_error)   \
        {                                                                                                             \
            std::stringstream buf(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(str));           \
            cast__ result;                                                                                            \
            buf >> result;                                                                                            \
            if(is_error != nullptr)                                                                                   \
            {                                                                                                         \
                *is_error = buf.fail()                                                                                \
                                || result > std::numeric_limits<type__>::max()                                        \
                                || result < std::numeric_limits<type__>::min();                                       \
            }                                                                                                         \
            if(result > std::numeric_limits<type__>::max())                                                           \
            {                                                                                                         \
                return std::numeric_limits<type__>::max();                                                            \
            }                                                                                                         \
            if(result < std::numeric_limits<type__>::min())                                                           \
            {                                                                                                         \
                return std::numeric_limits<type__>::min();                                                            \
            }                                                                                                         \
            return static_cast<type__>(result);                                                                       \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define VALUE_CONVERTER_TO_U8STRING_DEQUE(format__, type__)                                                          \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::basic_string<char8_t> to_string<format__, std::deque<type__>, char8_t>                           \
            (std::deque<type__> const& value, bool* is_error)                                                        \
        {                                                                                                             \
            return deque_to_string<format__, type__, char8_t>(                                                       \
                value                                                                                                 \
                , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(u8", ")                       \
                , is_error);                                                                                          \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define POINTER_CONVERTER_TO_U8STRING_DEQUE(format__, type__)                                                        \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::basic_string<char8_t> to_string<format__, std::deque<type__*>, char8_t>                          \
            (std::deque<type__*> const& value, bool* is_error)                                                       \
        {                                                                                                             \
            return deque_to_string<format__, type__, char8_t>(                                                       \
                value                                                                                                 \
                , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(u8", ")                       \
                , is_error);                                                                                          \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define VALUE_CONVERTER_FROM_U8STRING_DEQUE(format__, type__)                                                        \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::deque<type__> from_string<format__, std::deque<type__>, char8_t>                                \
            (std::basic_string<char8_t> const& str, bool* is_error)                                                   \
        {                                                                                                             \
            return string_to_deque<format__, type__, char8_t>(                                                       \
                str                                                                                                   \
                , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(u8", ")                       \
                , is_error);                                                                                          \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

//xml
#define VALUE_CONVERTER_TO_XML_U8STRING_BY_MEMBERFUNCTION(type__)                                                     \
    VALUE_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(xml, type__)
#define POINTER_CONVERTER_TO_XML_U8STRING_BY_MEMBERFUNCTION(type__)                                                   \
    POINTER_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(xml, type__)
#define VALUE_CONVERTER_FROM_XML_U8STRING_BY_MEMBERFUNCTION(type__)                                                   \
    VALUE_CONVERTER_FROM_U8STRING_BY_MEMBERFUNCTION(xml, type__)
#define VALUE_CONVERTER_TO_XML_U8STRING_BY_STREAM_PRECISION(type__, precision__)                                      \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION(xml, type__, precision__)
#define VALUE_CONVERTER_TO_XML_U8STRING_BY_STREAM(type__)                                                             \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION(xml, type__, -1)
#define VALUE_CONVERTER_FROM_XML_U8STRING_BY_STREAM(type__)                                                           \
    VALUE_CONVERTER_FROM_U8STRING_BY_STREAM(xml, type__)
#define VALUE_CONVERTER_TO_XML_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)                         \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION_CAST(xml, type__, precision__, cast__)
#define VALUE_CONVERTER_TO_XML_U8STRING_BY_STREAM_CAST(type__, cast__)                                                \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION_CAST(xml, type__, -1, cast__)
#define VALUE_CONVERTER_FROM_XML_U8STRING_BY_STREAM_CAST(type__, cast__)                                              \
    VALUE_CONVERTER_FROM_U8STRING_BY_STREAM_CAST(xml, type__, cast__)
#define VALUE_CONVERTER_TO_XML_U8STRING_DEQUE(type__)                                                                 \
    VALUE_CONVERTER_TO_U8STRING_DEQUE(xml, type__)
#define VALUE_CONVERTER_FROM_XML_U8STRING_DEQUE(type__)                                                               \
    VALUE_CONVERTER_FROM_U8STRING_DEQUE(xml, type__)
#define POINTER_CONVERTER_TO_XML_U8STRING_DEQUE(type__)                                                               \
    POINTER_CONVERTER_TO_U8STRING_DEQUE(xml, type__)

//json
#define VALUE_CONVERTER_TO_JSON_U8STRING_BY_MEMBERFUNCTION(type__)                                                    \
    VALUE_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(json, type__)
#define POINTER_CONVERTER_TO_JSON_U8STRING_BY_MEMBERFUNCTION(type__)                                                  \
    POINTER_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(json, type__)
#define VALUE_CONVERTER_FROM_JSON_U8STRING_BY_MEMBERFUNCTION(type__)                                                  \
    VALUE_CONVERTER_FROM_U8STRING_BY_MEMBERFUNCTION(json, type__)
#define VALUE_CONVERTER_TO_JSON_U8STRING_BY_STREAM_PRECISION(type__, precision__)                                     \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION(json, type__, precision__)
#define VALUE_CONVERTER_TO_JSON_U8STRING_BY_STREAM(type__)                                                            \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION(json, type__, -1)
#define VALUE_CONVERTER_FROM_JSON_U8STRING_BY_STREAM(type__)                                                          \
    VALUE_CONVERTER_FROM_U8STRING_BY_STREAM(json, type__)
#define VALUE_CONVERTER_TO_JSON_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)                        \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION_CAST(json, type__, precision__, cast__)
#define VALUE_CONVERTER_TO_JSON_U8STRING_BY_STREAM_CAST(type__, cast__)                                               \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION_CAST(json, type__, -1, cast__)
#define VALUE_CONVERTER_FROM_JSON_U8STRING_BY_STREAM_CAST(type__, cast__)                                             \
    VALUE_CONVERTER_FROM_U8STRING_BY_STREAM_CAST(json, type__, cast__)
#define VALUE_CONVERTER_TO_JSON_U8STRING_DEQUE(type__)                                                                \
    VALUE_CONVERTER_TO_U8STRING_DEQUE(json, type__)
#define VALUE_CONVERTER_FROM_JSON_U8STRING_DEQUE(type__)                                                             \
    VALUE_CONVERTER_FROM_U8STRING_DEQUE(json, type__)
#define POINTER_CONVERTER_TO_JSON_U8STRING_DEQUE(type__)                                                             \
    POINTER_CONVERTER_TO_U8STRING_DEQUE(JSON, type__)

//ini
#define VALUE_CONVERTER_TO_INI_U8STRING_BY_MEMBERFUNCTION(type__)                                                     \
    VALUE_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(ini, type__)
#define POINTER_CONVERTER_TO_INI_U8STRING_BY_MEMBERFUNCTION(type__)                                                   \
    POINTER_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(ini, type__)
#define VALUE_CONVERTER_FROM_INI_U8STRING_BY_MEMBERFUNCTION(type__)                                                   \
    VALUE_CONVERTER_FROM_U8STRING_BY_MEMBERFUNCTION(ini, type__)
#define VALUE_CONVERTER_TO_INI_U8STRING_BY_STREAM_PRECISION(type__, precision__)                                      \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION(ini, type__, precision__)
#define VALUE_CONVERTER_TO_INI_U8STRING_BY_STREAM(type__)                                                             \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION(ini, type__, -1)
#define VALUE_CONVERTER_FROM_INI_U8STRING_BY_STREAM(type__)                                                           \
    VALUE_CONVERTER_FROM_U8STRING_BY_STREAM(ini, type__)
#define VALUE_CONVERTER_TO_INI_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)                         \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION_CAST(ini, type__, precision__, cast__)
#define VALUE_CONVERTER_TO_INI_U8STRING_BY_STREAM_CAST(type__, cast__)                                                \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION_CAST(ini, type__, -1, cast__)
#define VALUE_CONVERTER_FROM_INI_U8STRING_BY_STREAM_CAST(type__, cast__)                                              \
    VALUE_CONVERTER_FROM_U8STRING_BY_STREAM_CAST(ini, type__, cast__)
#define VALUE_CONVERTER_TO_INI_U8STRING_DEQUE(type__)                                                                 \
    VALUE_CONVERTER_TO_U8STRING_DEQUE(ini, type__)
#define VALUE_CONVERTER_FROM_INI_U8STRING_DEQUE(type__)                                                              \
    VALUE_CONVERTER_FROM_U8STRING_DEQUE(ini, type__)
#define POINTER_CONVERTER_TO_INI_U8STRING_DEQUE(type__)                                                              \
    POINTER_CONVERTER_TO_U8STRING_DEQUE(ini, type__)

//display
#define VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_MEMBERFUNCTION(type__)                                                 \
    VALUE_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(display, type__)
#define POINTER_CONVERTER_TO_DISPLAY_U8STRING_BY_MEMBERFUNCTION(type__)                                               \
    POINTER_CONVERTER_TO_U8STRING_BY_MEMBERFUNCTION(display, type__)
#define VALUE_CONVERTER_FROM_DISPLAY_U8STRING_BY_MEMBERFUNCTION(type__)                                               \
    VALUE_CONVERTER_FROM_U8STRING_BY_MEMBERFUNCTION(display, type__)
#define VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_STREAM_PRECISION(type__, precision__)                                  \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION(display, type__, precision__)
#define VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_STREAM(type__)                                                         \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION(display, type__, -1)
#define VALUE_CONVERTER_FROM_DISPLAY_U8STRING_BY_STREAM(type__)                                                       \
    VALUE_CONVERTER_FROM_U8STRING_BY_STREAM(display, type__)
#define VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)                     \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION_CAST(display, type__, precision__, cast__)
#define VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_STREAM_CAST(type__, cast__)                                            \
    VALUE_CONVERTER_TO_U8STRING_BY_STREAM_PRECISION_CAST(display, type__, -1, cast__)
#define VALUE_CONVERTER_FROM_DISPLAY_U8STRING_BY_STREAM_CAST(type__, cast__)                                          \
    VALUE_CONVERTER_FROM_U8STRING_BY_STREAM_CAST(display, type__, cast__)
#define VALUE_CONVERTER_TO_DISPLAY_U8STRING_DEQUE(type__)                                                             \
    VALUE_CONVERTER_TO_U8STRING_DEQUE(display, type__)
#define VALUE_CONVERTER_FROM_DISPLAY_U8STRING_DEQUE(type__)                                                           \
    VALUE_CONVERTER_FROM_U8STRING_DEQUE(display, type__)
#define POINTER_CONVERTER_TO_DISPLAY_U8STRING_DEQUE(type__)                                                           \
    POINTER_CONVERTER_TO_U8STRING_DEQUE(display, type__)

////

#define VALUE_CONVERTER_TO_ALL_U8STRING_BY_MEMBERFUNCTION(type__)                                                     \
    VALUE_CONVERTER_TO_XML_U8STRING_BY_MEMBERFUNCTION(type__)                                                         \
    VALUE_CONVERTER_TO_JSON_U8STRING_BY_MEMBERFUNCTION(type__)                                                        \
    VALUE_CONVERTER_TO_INI_U8STRING_BY_MEMBERFUNCTION(type__)                                                         \
    VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_MEMBERFUNCTION(type__)

#define POINTER_CONVERTER_TO_ALL_U8STRING_BY_MEMBERFUNCTION(type__)                                                   \
    POINTER_CONVERTER_TO_XML_U8STRING_BY_MEMBERFUNCTION(type__)                                                       \
    POINTER_CONVERTER_TO_JSON_U8STRING_BY_MEMBERFUNCTION(type__)                                                      \
    POINTER_CONVERTER_TO_INI_U8STRING_BY_MEMBERFUNCTION(type__)                                                       \
    POINTER_CONVERTER_TO_DISPLAY_U8STRING_BY_MEMBERFUNCTION(type__)

#define VALUE_CONVERTER_FROM_ALL_U8STRING_BY_MEMBERFUNCTION(type__)                                                   \
    VALUE_CONVERTER_FROM_XML_U8STRING_BY_MEMBERFUNCTION(type__)                                                   	  \
    VALUE_CONVERTER_FROM_JSON_U8STRING_BY_MEMBERFUNCTION(type__)                                                      \
    VALUE_CONVERTER_FROM_INI_U8STRING_BY_MEMBERFUNCTION(type__)                                                       \
    VALUE_CONVERTER_FROM_DISPLAY_U8STRING_BY_MEMBERFUNCTION(type__)

#define VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_PRECISION(type__, precision__)                                      \
    VALUE_CONVERTER_TO_XML_U8STRING_BY_STREAM_PRECISION(type__, precision__)                                          \
    VALUE_CONVERTER_TO_JSON_U8STRING_BY_STREAM_PRECISION(type__, precision__)                                         \
    VALUE_CONVERTER_TO_INI_U8STRING_BY_STREAM_PRECISION(type__, precision__)                                          \
    VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_STREAM_PRECISION(type__, precision__)

#define VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM(type__)                                                             \
    VALUE_CONVERTER_TO_XML_U8STRING_BY_STREAM(type__)                                                                 \
    VALUE_CONVERTER_TO_JSON_U8STRING_BY_STREAM(type__)                                                                \
    VALUE_CONVERTER_TO_INI_U8STRING_BY_STREAM(type__)                                                                 \
    VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_STREAM(type__)

#define VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(type__)                                                           \
    VALUE_CONVERTER_FROM_XML_U8STRING_BY_STREAM(type__)                                                               \
    VALUE_CONVERTER_FROM_JSON_U8STRING_BY_STREAM(type__)                                                              \
    VALUE_CONVERTER_FROM_INI_U8STRING_BY_STREAM(type__)                                                               \
    VALUE_CONVERTER_FROM_DISPLAY_U8STRING_BY_STREAM(type__)

#define VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)                         \
    VALUE_CONVERTER_TO_XML_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)                             \
    VALUE_CONVERTER_TO_JSON_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)                            \
    VALUE_CONVERTER_TO_INI_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)                             \
    VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)

#define VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_CAST(type__, cast__)                                                \
    VALUE_CONVERTER_TO_XML_U8STRING_BY_STREAM_CAST(type__, cast__)                                                    \
    VALUE_CONVERTER_TO_JSON_U8STRING_BY_STREAM_CAST(type__, cast__)                                                   \
    VALUE_CONVERTER_TO_INI_U8STRING_BY_STREAM_CAST(type__, cast__)                                                    \
    VALUE_CONVERTER_TO_DISPLAY_U8STRING_BY_STREAM_CAST(type__, cast__)

#define VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM_CAST(type__, cast__)                                              \
    VALUE_CONVERTER_FROM_XML_U8STRING_BY_STREAM_CAST(type__, cast__)                                                  \
    VALUE_CONVERTER_FROM_JSON_U8STRING_BY_STREAM_CAST(type__, cast__)                                                 \
    VALUE_CONVERTER_FROM_INI_U8STRING_BY_STREAM_CAST(type__, cast__)                                                  \
    VALUE_CONVERTER_FROM_DISPLAY_U8STRING_BY_STREAM_CAST(type__, cast__)

#define VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(type__)                                                                 \
    VALUE_CONVERTER_TO_XML_U8STRING_DEQUE(type__)                                                                     \
    VALUE_CONVERTER_TO_JSON_U8STRING_DEQUE(type__)                                                                    \
    VALUE_CONVERTER_TO_INI_U8STRING_DEQUE(type__)                                                                     \
    VALUE_CONVERTER_TO_DISPLAY_U8STRING_DEQUE(type__)

#define VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(type__)                                                               \
    VALUE_CONVERTER_FROM_XML_U8STRING_DEQUE(type__)                                                                   \
    VALUE_CONVERTER_FROM_JSON_U8STRING_DEQUE(type__)                                                                  \
    VALUE_CONVERTER_FROM_INI_U8STRING_DEQUE(type__)                                                                   \
    VALUE_CONVERTER_FROM_DISPLAY_U8STRING_DEQUE(type__)

#define POINTER_CONVERTER_TO_ALL_U8STRING_DEQUE(type__)                                                               \
    POINTER_CONVERTER_TO_XML_U8STRING_DEQUE(type__)                                                                   \
    POINTER_CONVERTER_TO_JSON_U8STRING_DEQUE(type__)                                                                  \
    POINTER_CONVERTER_TO_INI_U8STRING_DEQUE(type__)                                                                   \
    POINTER_CONVERTER_TO_DISPLAY_U8STRING_DEQUE(type__)

/////////////////////////////////

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM(bool)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(bool)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(bool)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(bool)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_CAST(char, short)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM_CAST(char, short)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(char)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(char)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_CAST(unsigned char, unsigned short)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM_CAST(unsigned char, unsigned short)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(unsigned char)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(unsigned char)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_CAST(signed char, short)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM_CAST(signed char, short)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(signed char)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(signed char)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_CAST(wchar_t, short)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM_CAST(wchar_t, short)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(wchar_t)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(wchar_t)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_CAST(char8_t, short)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM_CAST(char8_t, short)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(char8_t)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(char8_t)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM(std::string)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(std::string)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(std::string)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(std::string)

//不提供 std::wstring std::u8string

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM(short)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(short)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(short)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(short)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM(unsigned short)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(unsigned short)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(unsigned short)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(unsigned short)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM(int)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(int)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(int)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(int)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM(unsigned int)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(unsigned int)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(unsigned int)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(unsigned int)

//long不适合跨平台，所以不提供

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM(long long)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(long long)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(long long)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(long long)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM(unsigned long long)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(unsigned long long)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(unsigned long long)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(unsigned long long)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_PRECISION(float, std::numeric_limits<float>::digits10)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(float)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(float)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(float)

VALUE_CONVERTER_TO_ALL_U8STRING_BY_STREAM_PRECISION(double, std::numeric_limits<double>::digits10)
VALUE_CONVERTER_FROM_ALL_U8STRING_BY_STREAM(double)
VALUE_CONVERTER_TO_ALL_U8STRING_DEQUE(double)
VALUE_CONVERTER_FROM_ALL_U8STRING_DEQUE(double)
