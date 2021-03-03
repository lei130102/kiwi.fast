#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

template<typename Body>
class send_http_post_request
{
public:
    send_http_post_request(std::u8string const& host, std::u8string const& port)
    {
        m_host = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(host);
        m_port = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(port);
    }

    void set_target(std::u8string const& target)
    {
        m_target = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(target);
    }

    void set_body(typename Body::value_type const& value)
    {
        m_body_value = value;
    }

    bool operator()(std::u8string* result = nullptr, std::string* error = nullptr)
    {
        try
        {
            boost::asio::io_context io_context_;

            boost::asio::ip::tcp::resolver resolver_(io_context_);
            boost::beast::tcp_stream stream_(io_context_);

            auto const results = resolver_.resolve(m_host, m_port);

            stream_.connect(results);

            boost::beast::http::request<Body> request_{
                boost::beast::http::verb::post, m_target, 11};
            request_.set(boost::beast::http::field::host, m_host);
            request_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            request_.body() = m_body_value;
            request_.prepare_payload();
            boost::beast::http::write(stream_, request_);

            boost::beast::flat_buffer flat_buffer_;
            boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_;
            boost::beast::http::read(stream_, flat_buffer_, response_);

            boost::beast::error_code ec;
            stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

            if(ec && ec != boost::beast::errc::not_connected)
            {
                if(error)
                {
                    *error = ec.message();
                }
                return false;
            }

            if(response_.result() == boost::beast::http::status::ok)
            {
                if(result != nullptr)
                {
                    *result = response_.body();
                }

                //统一初步判断什么情况返回false
                //if(1)
                //{
                //    return false;
                //}
            }

            return true;
        }
        catch(std::exception const& e)
        {
            if(error)
            {
                *error = e.what();
            }
            return false;
        }
    }

private:
    std::string m_host;
    std::string m_port;

    std::string m_target;
    typename Body::value_type m_body_value;
};


KIWI_FAST_CLOSE_UTILITY_NAMESPACE
