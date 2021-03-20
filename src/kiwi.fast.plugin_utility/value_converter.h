#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <deque>
#include <string>
#include <sstream>
#include <numeric>
#include <filesystem>
#include <optional>

//类型值与字符串之间的转换

//因为vector<bool>比较特殊，所以为了统一，都用deque

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

namespace value_converter
{
    namespace detail
    {
        template<typename CharType = char8_t>
        inline std::size_t deque_element_number_from_string(CharType const* str, CharType const* sep, bool* is_error = nullptr)
        {
            return 0;

            //bool is_error_ = false;
            //if (is_error != nullptr)
            //{
            //    *is_error = false;
            //}

            //std::size_t result = 0;
            //std::basic_string<CharType> str_ = str;

            //if (str_.empty())
            //{
            //    return result;
            //}

            //bool parsing = true;
            //typename std::basic_string<CharType>::size_type last_index = 0;
            //std::size_t element_index = 0;
            //while (parsing)
            //{
            //    typename std::basic_string<CharType>::size_type index = str_.find(sep, last_index);
            //    if (index == std::basic_string<CharType>::npos)
            //    {
            //        index = str_.size();
            //    }
            //    if (is_error_)
            //    {
            //        if (is_error != nullptr)
            //        {
            //            *is_error = true;
            //        }
            //        return result;
            //    }
            //    last_index = index + std::basic_string<CharType>(sep).size();
            //    if (index == str.size())
            //    {
            //        parsing = false;
            //    }
            //    else
            //    {
            //        ++result;
            //    }
            //}
            //return result;
        }

        template<typename CharType = char8_t>
        inline std::size_t deque_element_number_from_string(std::basic_string<CharType> const& str, std::basic_string<CharType> const& sep, bool* is_error = nullptr)
        {
            return deque_element_number_from_string(str.c_str(), sep.c_str(), is_error);
        }
    }

    template<typename T, typename CharType = char8_t>
    inline std::optional<std::basic_string<CharType>> object_to_string(T* obj, bool* is_error = nullptr);

    template<typename T, typename CharType = char8_t>
    inline void object_from_string(T* obj, CharType const* str, bool* is_error = nullptr);
    template<typename T, typename CharType = char8_t>
    inline void object_from_string(T* obj, std::basic_string<CharType> const& str, bool* is_error = nullptr)
    {
        object_from_string(obj, str.c_str(), is_error);
    }

    template<typename T, typename CharType = char8_t>
    inline std::optional<std::basic_string<CharType>> deque_to_string(std::deque<T*>* deq, CharType const* sep, bool* is_error = nullptr)
    {
        bool is_error_ = false;
        if (is_error != nullptr)
        {
            *is_error = false;
        }

        std::basic_string<CharType> result;
        for (auto iter = deq->begin(); iter != deq->end(); ++iter)
        {
            std::optional<std::basic_string<CharType>> str = object_to_string<T, CharType>(*iter, &is_error_);

            if (is_error_)
            {
                if (is_error != nullptr)
                {
                    *is_error = true;
                }
                return std::basic_string<CharType>();
            }

            if (str)
            {
                result += *str;
            }

            if ((iter + 1) != deq->end())
            {
                result += sep;
            }
        }
        return result;
    }
    template<typename T, typename CharType = char8_t>
    inline std::optional<std::basic_string<CharType>> deque_to_string(std::deque<T*>* deq, std::basic_string<CharType> const& sep, bool* is_error = nullptr)
    {
        return deque_to_string<T>(deq, sep.c_str(), is_error);
    }

    template<typename T, typename CharType = char8_t>
    inline void deque_from_string(std::deque<T*>* deq, CharType const* str, CharType const* sep, bool* is_error = nullptr)
    {
        bool is_error_ = false;
        if(is_error != nullptr)
        {
            *is_error = false;
        }

        std::basic_string<CharType> str_ = str;

        if(str_.empty())
        {
            return;
        }

        bool parsing = true;
        typename std::basic_string<CharType>::size_type last_index = 0;
        std::size_t element_index = 0;
        while(parsing)
        {
            typename std::basic_string<CharType>::size_type index = str_.find(sep, last_index);
            if(index == std::basic_string<CharType>::npos)
            {
                index = str_.size();
            }
            std::basic_string<CharType> token = str_.substr(last_index, (index - last_index));
            object_from_string<T, CharType>(deq->at(element_index), token, &is_error_);   //这里的索引操作可能抛出异常
            if(is_error_)
            {
                if(is_error != nullptr)
                {
                    *is_error = true;
                }
                return;
            }
            last_index = index + std::basic_string<CharType>(sep).size();
            if(index == str_.size())
            {
                parsing = false;
            }
            else
            {
                ++element_index;
            }
        }
    }
    template<typename T, typename CharType = char8_t>
    inline void deque_from_string(std::deque<T*>* deq, std::basic_string<CharType> const& str, std::basic_string<CharType> const& sep, bool* is_error = nullptr)
    {
        return deque_from_string(deq, str.c_str(), sep.c_str(), is_error);
    }
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

//BY_MEMBERFUNCTION和BY_STREAM两个宏不适合合并，因为输入参数不同

#define OBJECT_TO_U8STRING_BY_MEMBERFUNCTION(type__)                                                                  \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::optional<std::basic_string<char8_t>> object_to_string(type__* obj, bool* is_error)                \
        {                                                                                                             \
            if(obj == nullptr)                                                                                        \
            {                                                                                                         \
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error                        \
                    , u8"object_to_string 失败，指针为空");                                                            \
            }                                                                                                         \
            return obj->to_string<char8_t>(is_error);                                                                 \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define OBJECT_FROM_U8STRING_BY_MEMBERFUNCTION(type__)                                                                \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline void object_from_string(type__* obj, char8_t const* str, bool* is_error)                               \
        {                                                                                                             \
            if(obj == nullptr)                                                                                        \
            {                                                                                                         \
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error                        \
                    , u8"object_from_string 失败，指针为空");                                                          \
            }                                                                                                         \
            obj->from_string<char8_t>(str, is_error);                                                                 \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define OBJECT_TO_U8STRING_BY_STRING(type__)                                                                          \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::optional<std::basic_string<char8_t>> object_to_string(type__* obj, bool* is_error)                \
        {                                                                                                             \
            if(obj == nullptr)                                                                                        \
            {                                                                                                         \
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error                        \
                    , u8"object_to_string 失败，指针为空");                                                            \
            }                                                                                                         \
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(*obj);                       \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define OBJECT_FROM_U8STRING_BY_STRING(type__)                                                                        \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline void object_from_string(type__* obj, char8_t const* str, bool* is_error)                               \
        {                                                                                                             \
            if(obj == nullptr)                                                                                        \
            {                                                                                                         \
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error                        \
                    , u8"object_from_string 失败，指针为空");                                                          \
            }                                                                                                         \
            *obj = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<type__::value_type>(*obj);            \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define OBJECT_TO_U8STRING_BY_STREAM_PRECISION(type__, precision__)                                                   \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::optional<std::basic_string<char8_t>> object_to_string(type__* obj, bool* is_error)                \
        {                                                                                                             \
            if(obj == nullptr)                                                                                        \
            {                                                                                                         \
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error                        \
                    , u8"object_to_string 失败，指针为空");                                                            \
            }                                                                                                         \
            std::stringstream buf;                                                                                    \
            if(precision__ != -1)                                                                                     \
            {                                                                                                         \
                buf.precision(precision__);                                                                           \
            }                                                                                                         \
            buf << *obj;                                                                                              \
            if(is_error != nullptr)                                                                                   \
            {                                                                                                         \
                *is_error = buf.fail();                                                                               \
            }                                                                                                         \
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(buf.str());                  \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define OBJECT_TO_U8STRING_BY_STREAM(type__)                                                                          \
    OBJECT_TO_U8STRING_BY_STREAM_PRECISION(type__, -1)                                                   

#define OBJECT_TO_U8STRING_BY_STREAM_PRECISION_CAST(type__, precision__, cast__)                                      \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline std::optional<std::basic_string<char8_t>> object_to_string(type__* obj, bool* is_error)                \
        {                                                                                                             \
            if(obj == nullptr)                                                                                        \
            {                                                                                                         \
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error                        \
                    , u8"object_to_string 失败，指针为空");                                                            \
            }                                                                                                         \
            std::stringstream buf;                                                                                    \
            if(precision__ != -1)                                                                                     \
            {                                                                                                         \
                buf.precision(precision__);                                                                           \
            }                                                                                                         \
            buf << static_cast<cast__>(*obj);                                                                         \
            if(is_error != nullptr)                                                                                   \
            {                                                                                                         \
                *is_error = buf.fail();                                                                               \
            }                                                                                                         \
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(buf.str());                  \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define OBJECT_TO_U8STRING_BY_STREAM_CAST(type__, cast__)                                                             \
    OBJECT_TO_U8STRING_BY_STREAM_PRECISION_CAST(type__, -1, cast__)                                      

#define OBJECT_FROM_U8STRING_BY_STREAM(type__)                                                                        \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline void object_from_string(type__* obj, char8_t const* str, bool* is_error)                               \
        {                                                                                                             \
            if(obj == nullptr)                                                                                        \
            {                                                                                                         \
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error                        \
                    , u8"object_from_string 失败，指针为空");                                                          \
            }                                                                                                         \
            std::stringstream buf(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(str));           \
            buf >> *obj;                                                                                              \
            if(is_error != nullptr)                                                                                   \
            {                                                                                                         \
                *is_error = buf.fail();                                                                               \
            }                                                                                                         \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define OBJECT_FROM_U8STRING_BY_STREAM_CAST(type__, cast__)                                                           \
    KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE                                                                           \
    namespace value_converter                                                                                         \
    {                                                                                                                 \
        template<>                                                                                                    \
        inline void object_from_string(type__* obj, char8_t const* str, bool* is_error)                               \
        {                                                                                                             \
            if(obj == nullptr)                                                                                        \
            {                                                                                                         \
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error                        \
                    , u8"object_from_string 失败，指针为空");                                                          \
            }                                                                                                         \
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
                *obj = std::numeric_limits<type__>::max();                                                            \
            }                                                                                                         \
            else if(result < std::numeric_limits<type__>::min())                                                      \
            {                                                                                                         \
                *obj = std::numeric_limits<type__>::min();                                                            \
            }                                                                                                         \
            else                                                                                                      \
            {                                                                                                         \
                *obj = static_cast<type__>(result);                                                                   \
            }                                                                                                         \
        }                                                                                                             \
    }                                                                                                                 \
    KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

/////////////////////////////////

OBJECT_TO_U8STRING_BY_STREAM(bool)
OBJECT_FROM_U8STRING_BY_STREAM(bool)

OBJECT_TO_U8STRING_BY_STREAM_CAST(char, short)
OBJECT_FROM_U8STRING_BY_STREAM_CAST(char, short)

OBJECT_TO_U8STRING_BY_STREAM_CAST(unsigned char, unsigned short)
OBJECT_FROM_U8STRING_BY_STREAM_CAST(unsigned char, unsigned short)

OBJECT_TO_U8STRING_BY_STREAM_CAST(signed char, short)
OBJECT_FROM_U8STRING_BY_STREAM_CAST(signed char, short)

OBJECT_TO_U8STRING_BY_STREAM_CAST(wchar_t, short)
OBJECT_FROM_U8STRING_BY_STREAM_CAST(wchar_t, short)

OBJECT_TO_U8STRING_BY_STREAM_CAST(char8_t, short)
OBJECT_FROM_U8STRING_BY_STREAM_CAST(char8_t, short)

OBJECT_TO_U8STRING_BY_STRING(std::wstring)
OBJECT_FROM_U8STRING_BY_STRING(std::wstring)

OBJECT_TO_U8STRING_BY_STRING(std::u8string)
OBJECT_FROM_U8STRING_BY_STRING(std::u8string)

OBJECT_TO_U8STRING_BY_STRING(std::string)
OBJECT_FROM_U8STRING_BY_STRING(std::string)

OBJECT_TO_U8STRING_BY_STREAM(std::filesystem::path)
OBJECT_FROM_U8STRING_BY_STREAM(std::filesystem::path)

OBJECT_TO_U8STRING_BY_STREAM(short)
OBJECT_FROM_U8STRING_BY_STREAM(short)

OBJECT_TO_U8STRING_BY_STREAM(unsigned short)
OBJECT_FROM_U8STRING_BY_STREAM(unsigned short)

OBJECT_TO_U8STRING_BY_STREAM(int)
OBJECT_FROM_U8STRING_BY_STREAM(int)

OBJECT_TO_U8STRING_BY_STREAM(unsigned int)
OBJECT_FROM_U8STRING_BY_STREAM(unsigned int)

//long不适合跨平台，所以不提供

OBJECT_TO_U8STRING_BY_STREAM(long long)
OBJECT_FROM_U8STRING_BY_STREAM(long long)

OBJECT_TO_U8STRING_BY_STREAM(unsigned long long)
OBJECT_FROM_U8STRING_BY_STREAM(unsigned long long)

OBJECT_TO_U8STRING_BY_STREAM_PRECISION(float, std::numeric_limits<float>::digits10)
OBJECT_FROM_U8STRING_BY_STREAM(float)

OBJECT_TO_U8STRING_BY_STREAM_PRECISION(double, std::numeric_limits<float>::digits10)
OBJECT_FROM_U8STRING_BY_STREAM(double)
