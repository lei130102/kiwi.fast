#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <locale>
#include <cwchar> //mbstate_t

#ifdef KIWI_FAST_WINDOWS_API
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

namespace detail
{
    /*!
     * \brief The codecvt_char8_t_wchar_t class
     *
     * 仅部分实现了 codecvt ，甚至 do_in 和 do_out 成员函数也仅部分满足标准库规范，因为 to 缓冲区必须保存整个结果
     */
    class codecvt_char8_t_wchar_t : public std::codecvt<wchar_t, char8_t, std::mbstate_t>
    {
    public:
        explicit codecvt_char8_t_wchar_t(std::size_t refs = 0)
            : std::codecvt<wchar_t, char8_t, std::mbstate_t>(refs){}

    protected:
        virtual bool do_always_noconv() const throw()
        {
            return false;
        }

        virtual int do_encoding() const throw()
        {
            return 0;
        }

        virtual std::codecvt_base::result do_unshift(std::mbstate_t&, char8_t*,char8_t*,char8_t*&) const
        {
            return ok;
        }

        virtual int do_length(
                std::mbstate_t&, const char8_t*, const char8_t*, std::size_t) const
        {
            return 0;
        }

        virtual int do_max_length() const throw()
        {
            return 0;
        }

        virtual std::codecvt_base::result do_in(
                std::mbstate_t& state,
                const char8_t* from, const char8_t* from_end, const char8_t*& from_next,
                wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const
        {
            const char* from_ = reinterpret_cast<const char*>(from);
            const char* from_end_ = reinterpret_cast<const char*>(from_end);
            const char*& from_next_ = reinterpret_cast<const char*&>(from_next);

            UINT codepage = CP_UTF8;

            int count;
            if((count = ::MultiByteToWideChar(codepage, MB_PRECOMPOSED, from_, static_cast<int>(from_end_ - from_), to, static_cast<int>(to_end - to))) == 0)
            {
                return error;
            }

            from_next_ = from_end_;
            to_next = to + count;
            *to_next = static_cast<wchar_t>(0);
            return ok;
        }

        virtual std::codecvt_base::result do_out(
                std::mbstate_t& state,
                const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
                char8_t* to, char8_t* to_end, char8_t*& to_next) const
        {
            char* to_ = reinterpret_cast<char*>(to);
            char* to_end_ = reinterpret_cast<char*>(to_end);
            char*& to_next_ = reinterpret_cast<char*&>(to_next);

            UINT codepage = CP_UTF8;

            int count;
            if((count = ::WideCharToMultiByte(codepage, WC_NO_BEST_FIT_CHARS,
                from, static_cast<int>(from_end - from), to_, static_cast<int>(to_end_ - to_), 0, 0)) == 0)
            {
                return error;
            }

            from_next = from_end;
            to_next_ = to_ + count;
            *to_next_ = '\0';
            return ok;
        }
    };
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#endif //KIWI_FAST_WINDOWS_API
