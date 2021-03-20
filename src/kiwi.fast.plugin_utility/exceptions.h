#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>

#include <new>
#include <string>
#include <stdexcept>
#include <system_error>
#include <iostream>

#include <boost/exception/all.hpp>
#include <boost/preprocessor/seq/enum.hpp>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

/* 利用boost.exception减少std::exception派生类的创建 */
/* 利用虚继承boost.exception，而不是用boost::enable_error_info函数，可以使用boost.exception库提供的更多功能 */
/* 如果虚继承std::runtime_error和std::logic_error，那么调用boost::throw_exception()时无法编译，所以暂时一律都不再虚继承，只虚继承boost.exception */

using what_info = boost::error_info<struct tag_what, std::u8string>;

class exception : public boost::exception
{};

class bad_alloc : public std::bad_alloc, virtual public exception
{
public:
    bad_alloc() noexcept
    {}

    explicit bad_alloc(std::string const& descr) noexcept
        :m_message(descr)
    {}

    explicit bad_alloc(std::wstring const& descr)
        :m_message(code_conversion<char>(descr))
    {}

    bad_alloc(const bad_alloc&) noexcept = default;
    bad_alloc& operator=(const bad_alloc&) noexcept = default;

    virtual ~bad_alloc() noexcept
    {}

    virtual const char* what() const noexcept
    {
        return m_message.c_str();
    }

    static void throw_(const char* file, boost::throw_line::value_type line)
    {
        boost::throw_exception(bad_alloc() << boost::throw_file(file) << boost::throw_line(line));
    }

    static void throw_(const char* file, boost::throw_line::value_type line, std::u8string const& descr)
    {
        std::wstring descr_ = code_conversion<wchar_t>(descr);
        boost::throw_exception(bad_alloc(descr_) << what_info(descr) << boost::throw_file(file) << boost::throw_line(line));
    }

private:
    std::string m_message;
};

class runtime_error : public std::runtime_error, virtual public exception
{
public:
    explicit runtime_error(const char* descr)
        :std::runtime_error(descr)
    {}

    explicit runtime_error(std::string const& descr)
        :std::runtime_error(descr)
    {}

    explicit runtime_error(std::wstring const& descr)
        :std::runtime_error(code_conversion<char>(descr))
    {}

    runtime_error(const runtime_error&) noexcept = default;
    runtime_error& operator=(const runtime_error&) noexcept = default;

    virtual ~runtime_error() noexcept
    {}

    static void throw_(const char* file, boost::throw_line::value_type line, std::u8string const& descr)
    {
        std::wstring descr_ = code_conversion<wchar_t>(descr);
        boost::throw_exception(runtime_error(descr_) << what_info(descr) << boost::throw_file(file) << boost::throw_line(line));
    }
};

class logic_error : public std::logic_error, virtual public exception
{
public:
    explicit logic_error(std::string const& descr)
        :std::logic_error(descr)
    {}

    explicit logic_error(std::wstring const& descr)
        :std::logic_error(code_conversion<char>(descr))
    {}

    logic_error(const logic_error&) noexcept = default;
    logic_error& operator=(const logic_error&) noexcept = default;

    virtual ~logic_error() noexcept
    {}

    static void throw_(const char* file, boost::throw_line::value_type line, std::u8string const& descr)
    {
        std::wstring descr_ = code_conversion<wchar_t>(descr);
        boost::throw_exception(logic_error(descr_) << what_info(descr)  << boost::throw_file(file) << boost::throw_line(line));
    }
};

class system_error : public std::system_error, virtual public exception
{
public:
    system_error(std::error_code ec)
        :std::system_error(ec)
    {}

    system_error(std::error_code ec, std::string const& descr)
        :std::system_error(ec, descr)
    {}

    system_error(std::error_code ec, std::wstring const& descr)
        :std::system_error(ec, code_conversion<char>(descr))
    {}

    system_error(int ev, std::error_category const& ecat)
        :std::system_error(ev, ecat)
    {}

    system_error(int ev, std::error_category const& ecat, std::string const& descr)
        :std::system_error(ev, ecat, descr)
    {}

    system_error(int ev, std::error_category const& ecat, std::wstring const& descr)
        :std::system_error(ev, ecat, code_conversion<char>(descr))
    {}

    system_error(const system_error&) noexcept = default;
    system_error& operator=(const system_error&) noexcept = default;

    virtual ~system_error() noexcept
    {}

    static void throw_(const char* file, boost::throw_line::value_type line, std::u8string const& descr, std::error_code ec)
    {
        std::wstring descr_ = code_conversion<wchar_t>(descr);
        boost::throw_exception(system_error(ec, descr_) << what_info(descr) << boost::throw_file(file) << boost::throw_line(line));
    }

    static void throw_(const char* file, boost::throw_line::value_type line, std::u8string const& descr, int ev, std::error_category const& ecat)
    {
        std::wstring descr_ = code_conversion<wchar_t>(descr);
        boost::throw_exception(system_error(ev, ecat, descr_) << what_info(descr) << boost::throw_file(file) << boost::throw_line(line));
    }
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

#define KIWI_FAST_THROW(ex)                                          							   \
    ex::throw_(__FILE__, static_cast<boost::throw_line::value_type>(__LINE__))

#define KIWI_FAST_THROW_DESCR(ex, descr)                             							   \
    ex::throw_(__FILE__, static_cast<boost::throw_line::value_type>(__LINE__), (descr))

#define KIWI_FAST_THROW_DESCR_PARAMS(ex, descr, params)                                            \
    ex::throw_(__FILE__, static_cast<boost::throw_line::value_type>(__LINE__), (descr), BOOST_PP_SEQ_ENUM(params))
