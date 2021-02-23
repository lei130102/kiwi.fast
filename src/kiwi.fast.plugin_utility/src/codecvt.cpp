#include <kiwi.fast.plugin_utility/codecvt.h>

#include <kiwi.fast.plugin_utility/detail/config.h>

namespace std
{
#if defined(__MINGW64__)
    locale::id codecvt<wchar_t, char8_t, mbstate_t>::id;

    codecvt<wchar_t, char8_t, mbstate_t>::codecvt(size_t __refs)
        :__codecvt_abstract_base<wchar_t, char8_t, mbstate_t>(__refs),
          _M_c_locale_codecvt(_S_get_c_locale())
    {}

    codecvt<wchar_t, char8_t, mbstate_t>::codecvt(__c_locale __cloc, size_t __refs)
        :__codecvt_abstract_base<wchar_t, char8_t, mbstate_t>(__refs),
          _M_c_locale_codecvt(_S_clone_c_locale(__cloc))
    {}

    codecvt<wchar_t, char8_t, mbstate_t>::~codecvt()
    {
        _S_destroy_c_locale(_M_c_locale_codecvt);
    }

    codecvt_base::result codecvt<wchar_t, char8_t, mbstate_t>::do_unshift(state_type&, extern_type* __to, extern_type*, extern_type*& __to_next) const
    {
        __to_next = __to;
        return noconv;
    }

    bool codecvt<wchar_t, char8_t, mbstate_t>::do_always_noconv() const throw()
    {
        return false;
    }

    int codecvt<wchar_t, char8_t, mbstate_t>::do_length(state_type&, const extern_type* __from,
          const extern_type* __end, size_t __max) const
    {
        return 0;
    }

    int codecvt<wchar_t, char8_t, mbstate_t>::do_max_length() const throw()
    {
        return 0;
    }

    int codecvt<wchar_t, char8_t, mbstate_t>::do_encoding() const throw()
    {
        return 0;
    }

    codecvt_base::result codecvt<wchar_t, char8_t, mbstate_t>::do_in(state_type& __state,
       const extern_type* __from, const extern_type* __from_end,
       const extern_type*& __from_next,
       intern_type* __to, intern_type* __to_end,
       intern_type*& __to_next) const
    {
        return error;
    }

    codecvt_base::result codecvt<wchar_t, char8_t, mbstate_t>::do_out(state_type& __state, const intern_type* __from,
       const intern_type* __from_end, const intern_type*& __from_next,
       extern_type* __to, extern_type* __to_end,
       extern_type*& __to_next) const
    {
        return error;
    }
#elif defined(_MSC_VER)

#endif
}
