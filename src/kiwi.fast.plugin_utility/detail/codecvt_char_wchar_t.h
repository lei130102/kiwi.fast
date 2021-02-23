#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <locale>
#include <cwchar>  //mbstate_t

#ifdef KIWI_FAST_WINDOWS_API
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

namespace detail
{
    /*!
     * \brief The codecvt_char_wchar_t class
     *
     * 仅部分实现了 codecvt ，甚至 do_in 和 do_out 成员函数也仅部分满足标准库规范，因为 to 缓冲区必须保存整个结果
     */
    class codecvt_char_wchar_t : public std::codecvt<wchar_t, char, std::mbstate_t>
    {
    public:
        explicit codecvt_char_wchar_t(std::size_t refs = 0)
            : std::codecvt<wchar_t, char, std::mbstate_t>(refs){}

    protected:
        virtual bool do_always_noconv() const throw() {return false;}

        /*!
         * \brief do_encoding seems safest to assume variable number of characters since we don't actually know what codepage is active
         * \return
         */
        virtual int do_encoding() const throw() {return 0;}

        virtual std::codecvt_base::result do_unshift(
                    std::mbstate_t&, char* /*from*/, char* /*to*/, char*& /*next*/) const
            {return ok;}

        virtual int do_length(
                    std::mbstate_t&, const char* /*from*/, const char* /*from_end*/, std::size_t /*max*/) const
            {return 0;}

        virtual int do_max_length() const throw()
            {return 0;}

        virtual std::codecvt_base::result do_in(
                    std::mbstate_t& state,
                    const char* from, const char* from_end, const char*& from_next,
                    wchar_t* to, wchar_t* to_end, wchar_t*& to_next
            ) const
        {
            UINT codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

            int count;
            if((count = ::MultiByteToWideChar(
                    codepage, MB_PRECOMPOSED, from, static_cast<int>(from_end - from), to, static_cast<int>(to_end - to)
                    )) == 0)
            {
                return error;//转换失败
            }

            from_next = from_end;
            to_next = to + count;
            *to_next = L'\0';
            return ok;
        }

        virtual std::codecvt_base::result do_out(
                    std::mbstate_t& state,
                    const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
                    char* to, char* to_end, char*& to_next
            ) const
        {
            UINT codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

            int count;
            if((count = ::WideCharToMultiByte(codepage, WC_NO_BEST_FIT_CHARS,
                from, static_cast<int>(from_end - from), to, static_cast<int>(to_end - to), 0, 0)) == 0)
            {
                return error;//转换失败
            }

            from_next = from_end;
            to_next = to + count;
            *to_next = '\0';
            return ok;
        }
    };
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#endif //KIWI_FAST_WINDOWS_API
