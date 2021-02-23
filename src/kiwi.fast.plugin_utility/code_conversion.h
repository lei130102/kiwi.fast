#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <string>
#include <type_traits>
#include <locale>
#include <vector>
#include <algorithm>

#include <boost/exception/all.hpp>

#include <kiwi.fast.plugin_utility/codecvt.h>

#include <kiwi.fast.plugin_utility/detail/type_traits/is_character_type.h>
#include <kiwi.fast.plugin_utility/detail/codecvt_char_wchar_t.h>
#include <kiwi.fast.plugin_utility/detail/codecvt_char8_t_wchar_t.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

KIWI_FAST_ANONYMOUS_NAMESPACE
{
    std::locale locale_locale()
    {
        std::locale global_loc = std::locale();

        return std::locale(global_loc, new detail::codecvt_char_wchar_t);
    }

    std::locale utf8_locale()
    {
        std::locale global_loc = std::locale();

        return std::locale(global_loc, new detail::codecvt_char8_t_wchar_t);
    }

    std::locale kiwi_fast_defined()
    {
        std::locale global_loc = std::locale();

        return std::locale(global_loc, new detail::codecvt_char_wchar_t);
        /* std::locale不支持std::codecvt<wchar_t,char8_t,mbstate_t> */
    }

    template<typename _Elem, typename _Byte>
    inline std::codecvt_base::result in_out(
                std::codecvt<_Elem, _Byte, std::mbstate_t> const& fac,
                std::mbstate_t& state,
                _Byte const*& pSrcBegin,
                _Byte const* pSrcEnd,
                _Elem *& pDstBegin,
                _Elem * pDstEnd
                )
    {
        return fac.in(state, pSrcBegin, pSrcEnd, pSrcBegin, pDstBegin, pDstEnd, pDstBegin);
    }

    template<typename _Elem, typename _Byte>
    inline std::codecvt_base::result in_out(
                std::codecvt<_Elem, _Byte, std::mbstate_t> const& fac,
                std::mbstate_t& state,
                _Elem const*& pSrcBegin,
                _Elem const* pSrcEnd,
                _Byte *& pDstBegin,
                _Byte * pDstEnd
                )
    {
        return fac.out(state, pSrcBegin, pSrcEnd, pSrcBegin, pDstBegin, pDstEnd, pDstBegin);
    }

    template<typename SourceCharT, typename TargetCharT, typename FacetT>
    inline std::size_t code_convert_by_in_out(
                SourceCharT const* begin, SourceCharT const* end
                , std::basic_string<TargetCharT>& converted, std::size_t max_size
                , FacetT const& fac)
    {
        using state_type = typename FacetT::state_type;

        std::vector<TargetCharT> converted_buffer;
        converted_buffer.resize(4 * (end - begin));

        SourceCharT const* const original_begin = begin;
        state_type state = state_type();
        std::size_t buf_size = (std::min)(max_size, converted_buffer.size());
        while(begin != end && buf_size > 0u)
        {
            TargetCharT* dest = converted_buffer.data();
            std::codecvt_base::result res = in_out(
                        fac,
                        state,
                        begin,
                        end,
                        dest,
                        dest + buf_size
                        );

            switch(res)
            {
                case std::codecvt_base::ok:
                /* 所有字符都被成功转换 */
                /* 注意：当源缓冲区仅被部分消耗时，MSVC 11也会返回OK，因此还需要检查begin指针是否到达末尾 */
                converted.append(converted_buffer.data(), dest);
                max_size -= dest - converted_buffer.data();
                break;

            case std::codecvt_base::noconv:
                {
                    /* 除非两种字符类型实际上是等效的，否则不可能 */
                }
                goto done;

            case std::codecvt_base::partial:
                /* 一些字符被转换，而另一些没有 */
                if(dest != converted_buffer.data())
                {
                    /* 进行了一些转换，因此可能目标缓冲区不够长 */
                    converted.append(converted_buffer.data(), dest);
                    max_size -= dest - converted_buffer.data();

                    /* 然后继续下一部分 */
                    break;
                }
                else
                {
                    /* 没有任何字符被转换 */
                    if(begin == end)
                    {
                        goto done;
                    }

                    /* 可能是源缓冲区的尾部仅包含最后一个字符的一部分，在这种情况下，故意向下执行到default:里面以引发异常 */
                }

            default: /* std::codecvt_base::error */
                /* 注意要用boost::exception& e捕获才能提取附加信息 */
                int i=0;
                i++;
                //boost::throw_exception(boost::enable_error_info(std::runtime_error("convert error")) << boost::throw_file(__FILE__) << boost::throw_line(__LINE__));
            }

            buf_size = (std::min)(max_size, converted_buffer.size());
        }

        done:
        return static_cast<std::size_t>(begin - original_begin);
    }

    bool code_convert_impl(
            wchar_t const* str_source, std::size_t len_source
            , std::string& str_target, std::size_t max_size_target
            , std::locale const& loc = kiwi_fast_defined())
    {
        return code_convert_by_in_out(
                    str_source, str_source + len_source
                    , str_target, max_size_target
                    , std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc)) == len_source;
    }

    bool code_convert_impl(
            char const* str_source, std::size_t len_source
            , std::wstring& str_target, std::size_t max_size_target
            , std::locale const& loc = kiwi_fast_defined())
    {
        return code_convert_by_in_out(
                    str_source, str_source + len_source
                    , str_target, max_size_target
                    , std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc)) == len_source;
    }

    bool code_convert_impl(
            wchar_t const* str_source, std::size_t len_source
            , std::u8string& str_target, std::size_t max_size_target
            , std::codecvt<wchar_t, char8_t, std::mbstate_t> const& c = detail::codecvt_char8_t_wchar_t())
    {
        return code_convert_by_in_out(
                    str_source, str_source + len_source
                    , str_target, max_size_target
                    , c) == len_source;
    }

    bool code_convert_impl(
            char8_t const* str_source, std::size_t len_source
            , std::wstring& str_target, std::size_t max_size_target
            , std::codecvt<wchar_t, char8_t, std::mbstate_t> const& c = detail::codecvt_char8_t_wchar_t())
    {
        return code_convert_by_in_out(
                    str_source, str_source + len_source
                    , str_target, max_size_target
                    , c) == len_source;
    }
}

//1.sizeof(SourceCharT) == sizeof(TargetCharT)

template<typename SourceCharT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) == sizeof(TargetCharT), bool>
code_convert(SourceCharT const* str_source, std::size_t len_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target, std::size_t max_size_target
             ,std::locale const& loc = kiwi_fast_defined())
{
    if constexpr(std::is_same_v<SourceCharT, char> && std::is_same_v<TargetCharT, char8_t>)
    {
        return false;
    }
    else if constexpr(std::is_same_v<SourceCharT, char8_t> && std::is_same_v<TargetCharT, char>)
    {
        return false;
    }
    else
    {
        std::size_t size_left = str_target.size() < max_size_target ? max_size_target - str_target.size() : static_cast<std::size_t>(0u);
        const bool overflow = len_source > size_left;

        str_target.append(reinterpret_cast<TargetCharT const*>(str_source), overflow ? size_left : len_source);

        return !overflow;
    }
}

template<typename SourceCharT, typename SourceTraitsT, typename SourceAllocatorT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) == sizeof(TargetCharT), bool>
code_convert(std::basic_string<SourceCharT, SourceTraitsT, SourceAllocatorT> const& str_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target, std::size_t max_size_target
             ,std::locale const& loc = kiwi_fast_defined())
{
    return code_convert(str_source.data(), str_source.size(), str_target, max_size_target, loc);
}

template<typename SourceCharT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) == sizeof(TargetCharT), bool>
code_convert(SourceCharT const* str_source, std::size_t len_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target
             ,std::locale const& loc = kiwi_fast_defined())
{
    return code_convert(str_source, len_source, str_target, str_target.max_size(), loc);
}

template<typename SourceCharT, typename SourceTraitsT, typename SourceAllocatorT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) == sizeof(TargetCharT), bool>
code_convert(std::basic_string<SourceCharT, SourceTraitsT, SourceAllocatorT> const& str_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target
             ,std::locale const& loc = kiwi_fast_defined())
{
    return code_convert(str_source.data(), str_source.size(), str_target, str_target.max_size(), loc);
}

//2.sizeof(SourceCharT) != sizeof(TargetCharT)

template<typename SourceCharT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) != sizeof(TargetCharT), bool>
code_convert(SourceCharT const* str_source, std::size_t len_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target, std::size_t max_size_target
             ,std::locale const& loc = kiwi_fast_defined())
{
    return code_convert_impl(str_source, len_source, str_target, max_size_target, loc);
}

template<typename SourceCharT, typename SourceTraitsT, typename SourceAllocatorT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) != sizeof(TargetCharT), bool>
code_convert(std::basic_string<SourceCharT, SourceTraitsT, SourceAllocatorT> const& str_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target, std::size_t max_size_target
             ,std::locale const& loc = kiwi_fast_defined())
{
    return code_convert(str_source.data(), str_source.size(), str_target, max_size_target, loc);
}

template<typename SourceCharT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) != sizeof(TargetCharT), bool>
code_convert(SourceCharT const* str_source, std::size_t len_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target
             ,std::locale const& loc = kiwi_fast_defined())
{
    return code_convert(str_source, len_source, str_target, str_target.max_size(), loc);
}

template<typename SourceCharT, typename SourceTraitsT, typename SourceAllocatorT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) != sizeof(TargetCharT), bool>
code_convert(std::basic_string<SourceCharT, SourceTraitsT, SourceAllocatorT> const& str_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target
             ,std::locale const& loc = kiwi_fast_defined())
{
    return code_convert(str_source.data(), str_source.size(), str_target, str_target.max_size(), loc);
}

/* 针对使用std::codecvt<wchar_t, char8_t, std::mbstate_t>类型参数的情况(std::locale不支持该平面) */

template<typename SourceCharT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) != sizeof(TargetCharT), bool>
code_convert(SourceCharT const* str_source, std::size_t len_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target, std::size_t max_size_target
             ,std::codecvt<wchar_t, char8_t, std::mbstate_t> const& c = detail::codecvt_char8_t_wchar_t())
{
    return code_convert_impl(str_source, len_source, str_target, max_size_target, c);
}

template<typename SourceCharT, typename SourceTraitsT, typename SourceAllocatorT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) != sizeof(TargetCharT), bool>
code_convert(std::basic_string<SourceCharT, SourceTraitsT, SourceAllocatorT> const& str_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target, std::size_t max_size_target
             ,std::codecvt<wchar_t, char8_t, std::mbstate_t> const& c = detail::codecvt_char8_t_wchar_t())
{
    return code_convert(str_source.data(), str_source.size(), str_target, max_size_target, c);
}

template<typename SourceCharT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) != sizeof(TargetCharT), bool>
code_convert(SourceCharT const* str_source, std::size_t len_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target
             ,std::codecvt<wchar_t, char8_t, std::mbstate_t> const& c = detail::codecvt_char8_t_wchar_t())
{
    return code_convert(str_source, len_source, str_target, str_target.max_size(), c);
}

template<typename SourceCharT, typename SourceTraitsT, typename SourceAllocatorT
         , typename TargetCharT, typename TargetTraitsT, typename TargetAllocatorT>
inline std::enable_if_t<detail::type_traits::is_character_type_v<SourceCharT>
                        && detail::type_traits::is_character_type_v<TargetCharT>
                        && sizeof(SourceCharT) != sizeof(TargetCharT), bool>
code_convert(std::basic_string<SourceCharT, SourceTraitsT, SourceAllocatorT> const& str_source
             ,std::basic_string<TargetCharT, TargetTraitsT, TargetAllocatorT>& str_target
             ,std::codecvt<wchar_t, char8_t, std::mbstate_t> const& c = detail::codecvt_char8_t_wchar_t())
{
    return code_convert(str_source.data(), str_source.size(), str_target, str_target.max_size(), c);
}

inline std::string to_local(std::wstring const& str, std::locale const& loc = kiwi_fast_defined())
{
    std::string result;
    code_convert_impl(str.c_str(), str.size(), result, result.max_size(), loc);
    return result;
}

inline std::u8string to_utf8(std::wstring const& str, std::codecvt<wchar_t, char8_t, std::mbstate_t> const& c = detail::codecvt_char8_t_wchar_t())
{
    std::u8string result;
    code_convert_impl(str.c_str(), str.size(), result, result.max_size(), c);
    return result;
}

inline std::wstring to_wide(std::string const& str, std::locale const& loc = kiwi_fast_defined())
{
    std::wstring result;
    code_convert_impl(str.c_str(), str.size(), result, result.max_size(), loc);
    return result;
}

inline std::wstring to_wide(std::u8string const& str, std::codecvt<wchar_t, char8_t, std::mbstate_t> const& c = detail::codecvt_char8_t_wchar_t())
{
    std::wstring result;
    code_convert_impl(str.c_str(), str.size(), result, result.max_size(), c);
    return result;
}

//

inline std::string to_local(std::u8string const& str)
{
    return to_local(to_wide(str));
}

inline std::u8string to_utf8(std::string const& str)
{
    return to_utf8(to_wide(str));
}


KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
