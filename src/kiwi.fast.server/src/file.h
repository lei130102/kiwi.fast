#pragma once

#include <kiwi.fast.server/detail/config.h>

#include <boost/beast/core/error.hpp>
#include <boost/beast/core/file_base.hpp>
#include <boost/core/exchange.hpp>

#include <cstdio>
#include <cstdint>

#include <limits>

/*!
 * \brief The file class 参考boost::beast::file_stdio
 */
class file
{
    FILE* f_ = nullptr;

public:

    using native_handle_type = FILE*;

    ~file()
    {
        if(f_)
            fclose(f_);
    }

    file() = default;

    file(file&& other)
        :f_(boost::exchange(other.f_, nullptr))
    {

    }

    file& operator=(file&& other)
    {
        if(&other == this)
        {
            return *this;
        }
        if(f_)
            fclose(f_);
        f_ = other.f_;
        other.f_ = nullptr;
        return *this;
    }

    FILE* native_handle() const
    {
        return f_;
    }

    void native_handle(FILE* f)
    {
        if(f_)
            fclose(f_);
        f_ = f;
    }

    bool is_open() const
    {
        return f_ != nullptr;
    }

    void close(boost::beast::error_code& ec)
    {
        if(f_)
        {
            int failed = fclose(f_);
            f_ = nullptr;
            if(failed)
            {
                //ec.assign(errno, boost::generic_category());
                return;
            }
        }
        ec = {};
    }

    void open(char const* path, boost::beast::file_mode mode, boost::beast::error_code& ec)
    {
        if(f_)
        {
            fclose(f_);
            f_ = nullptr;
        }
        char const* s;
        switch(mode)
        {
        default:
        case boost::beast::file_mode::read:
            s = "rb";
            break;
        case boost::beast::file_mode::scan:
            s = "rb";
            break;
        case boost::beast::file_mode::write:
            s = "wb+";
            break;
        case boost::beast::file_mode::write_new:
            s = "wbx";
            break;
        case boost::beast::file_mode::write_existing:
            s = "rb+";
            break;
        case boost::beast::file_mode::append:
            s = "ab";
            break;
        case boost::beast::file_mode::append_existing:
        {
            auto const f0 = fopen64(path, "rb+");
            if(!f0)
            {
                //ec.assign(errno, generic_category());
                return;
            }
            std::fclose(f0);
            s = "ab";
            break;
        }
        }
        f_ = fopen64(path, s);
        if(!f_)
        {
            //ec.assign(errno, generic_category());
            return;
        }
        ec = {};
    }

    std::uint64_t size(boost::beast::error_code& ec) const
    {
        if(!f_)
        {
            ec = make_error_code(boost::beast::errc::bad_file_descriptor);
            return 0;
        }
        _off64_t pos = ftello64(f_);
        if(pos == -1LL)
        {
            //ec.assign(errno, generic_category());
            return 0;
        }
        int result = fseeko64(f_, 0, SEEK_END);
        if(result != 0)
        {
            //ec.assign(errno, generic_category());
            return 0;
        }
        _off64_t size = ftello64(f_);
        if(size == -1LL)
        {
            //ec.assign(errno, generic_category());
            fseeko64(f_, pos, SEEK_SET);
            return 0;
        }
        result = fseeko64(f_, pos, SEEK_SET);
        if(result != 0)
        {
            //ec.assign(errno, generic_category());
        }
        else
        {
            ec = {};
        }
        return size;
    }

    std::uint64_t pos(boost::beast::error_code& ec) const
    {
        if(!f_)
        {
            ec = make_error_code(boost::beast::errc::bad_file_descriptor);
            return 0;
        }
        _off64_t pos = ftello64(f_);
        if(pos == -1LL)
        {
            //ec.assign(errno, generic_category());
            return 0;
        }
        ec = {};
        return pos;
    }

    void seek(std::uint64_t offset, boost::beast::error_code& ec)
    {
        if(!f_)
        {
            ec = make_error_code(boost::beast::errc::bad_file_descriptor);
            return;
        }
        if(offset > (std::numeric_limits<long long>::max)())
        {
            ec = make_error_code(boost::beast::errc::invalid_seek);
            return;
        }
        int result = fseeko64(f_,
            static_cast<long long>(offset), SEEK_SET);
        if(result != 0)
        {
            //ec.assign(errno, generic_category());
        }
        else
        {
            ec = {};
        }
    }

    std::size_t read(void* buffer, std::size_t n, boost::beast::error_code& ec) const
    {
        if(!f_)
        {
            ec = make_error_code(boost::beast::errc::bad_file_descriptor);
            return 0;
        }
        auto nread = fread(buffer, 1, n, f_);
        if(ferror(f_))
        {
            //ec.assign(errno, generic_category());
            return 0;
        }
        return nread;
    }

    std::size_t write(void const* buffer, std::size_t n, boost::beast::error_code& ec)
    {
        if(!f_)
        {
            ec = make_error_code(boost::beast::errc::bad_file_descriptor);
            return 0;
        }
        auto nwritten = fwrite(buffer, 1, n, f_);
        if(std::ferror(f_))
        {
            //ec.assign(errno, generic_category());
            return 0;
        }
        return nwritten;
    }
};
