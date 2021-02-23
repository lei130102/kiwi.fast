#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <codecvt>

namespace std
{
#if defined(__MINGW64__)
    template<>
      class codecvt<wchar_t, char8_t, mbstate_t>
      : public __codecvt_abstract_base<wchar_t, char8_t, mbstate_t>
      {
        friend class messages<wchar_t>;

      public:
        // Types:
        typedef wchar_t			intern_type;
        typedef char8_t			extern_type;
        typedef mbstate_t		state_type;

      protected:
        __c_locale			_M_c_locale_codecvt;

      public:
        static locale::id			id;

        explicit
        codecvt(size_t __refs = 0);

        explicit
        codecvt(__c_locale __cloc, size_t __refs = 0);

      protected:
        virtual
        ~codecvt();

        virtual result
        do_out(state_type& __state, const intern_type* __from,
           const intern_type* __from_end, const intern_type*& __from_next,
           extern_type* __to, extern_type* __to_end,
           extern_type*& __to_next) const;

        virtual result
        do_unshift(state_type& __state,
           extern_type* __to, extern_type* __to_end,
           extern_type*& __to_next) const;

        virtual result
        do_in(state_type& __state,
           const extern_type* __from, const extern_type* __from_end,
           const extern_type*& __from_next,
           intern_type* __to, intern_type* __to_end,
           intern_type*& __to_next) const;

        virtual
        int do_encoding() const throw();

        virtual
        bool do_always_noconv() const throw();

        virtual
        int do_length(state_type&, const extern_type* __from,
              const extern_type* __end, size_t __max) const;

        virtual int
        do_max_length() const throw();
      };
#elif defined(_MSC_VER)
    template <>
    class codecvt<wchar_t, char8_t, mbstate_t> : public codecvt_base {
    public:
        using intern_type = wchar_t;
        using extern_type = char8_t;
        using state_type = mbstate_t;

        result __CLR_OR_THIS_CALL in(mbstate_t& _State, const char8_t* _First1, const char8_t* _Last1, const char8_t*& _Mid1,
            wchar_t* _First2, wchar_t* _Last2, wchar_t*& _Mid2) const
        {
            return do_in(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
        }

        result __CLR_OR_THIS_CALL out(mbstate_t& _State, const wchar_t* _First1, const wchar_t* _Last1, const wchar_t*& _Mid1,
            char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const
        {
            return do_out(_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
        }

        result __CLR_OR_THIS_CALL unshift(mbstate_t& _State, char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const {

            return do_unshift(_State, _First2, _Last2, _Mid2);
        }

        int __CLR_OR_THIS_CALL length(mbstate_t& _State, const char8_t* _First1, const char8_t* _Last1, size_t _Count) const {

            return do_length(_State, _First1, _Last1, _Count);
        }

        __PURE_APPDOMAIN_GLOBAL static locale::id id;

        explicit __CLR_OR_THIS_CALL codecvt(size_t _Refs = 0) : codecvt_base(_Refs) {
            _BEGIN_LOCINFO(_Lobj)
                _Init(_Lobj);
            _END_LOCINFO()
        }

        explicit __CLR_OR_THIS_CALL codecvt(const _Locinfo& _Lobj, size_t _Refs = 0) : codecvt_base(_Refs) {
            _Init(_Lobj);
        }

        static size_t __CLRCALL_OR_CDECL _Getcat(const locale::facet** _Ppf = nullptr, const locale* _Ploc = nullptr) {

            if (_Ppf && !*_Ppf) {
                *_Ppf = new codecvt(_Locinfo(_Ploc->c_str()));
            }

            return _X_CTYPE;
        }

    protected:
        virtual __CLR_OR_THIS_CALL ~codecvt() noexcept
        {}

        void __CLR_OR_THIS_CALL _Init(const _Locinfo& _Lobj) { // initialize from _Lobj
            _Cvt = _Lobj._Getcvt();
        }

        virtual result __CLR_OR_THIS_CALL do_in(mbstate_t&, const char8_t* _First1, const char8_t* _Last1, const char8_t*& _Mid1,
            wchar_t* _First2, wchar_t* _Last2, wchar_t*& _Mid2) const {

            return error;

        }

        virtual result __CLR_OR_THIS_CALL do_out(mbstate_t& _State, const wchar_t* _First1, const wchar_t* _Last1,
            const wchar_t*& _Mid1, char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const {

            return error;
        }

        virtual result __CLR_OR_THIS_CALL do_unshift(mbstate_t& _State, char8_t* _First2, char8_t* _Last2, char8_t*& _Mid2) const {

            _Mid2 = _First2;
            return noconv;
        }

        virtual int __CLR_OR_THIS_CALL do_length(
            mbstate_t& _State, const char8_t* _First1, const char8_t* _Last1, size_t _Count) const {

            return 0;
        }

        virtual bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept override {

            return false;
        }

        virtual int __CLR_OR_THIS_CALL do_max_length() const noexcept override {

            return 0;
        }

        virtual int __CLR_OR_THIS_CALL do_encoding() const noexcept override {

            return 0;
        }

    private:
        _Locinfo::_Cvtvec _Cvt;
    };
#endif
}
