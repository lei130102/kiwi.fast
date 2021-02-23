#pragma once

#include <kiwi.fast.server/detail/config.h>

#include <kiwi.fast.server/src/file.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>

#include <kiwi.fast.model/detail/station_count.h>
#include <kiwi.fast.model/detail/station_count_producer_consumer.h>

#include <kiwi.fast.utility/threadpool.h>

#define BOOST_USE_WINDOWS_H
#include <boost/process.hpp>

#include <boost/beast/http/message.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/version.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


#include <sqlite3.h>

#include <vector>
#include <string>
#include <iterator>

namespace
{
    bool url_decode(std::string const& in, std::u8string& out_w)
    {
        out_w.clear();

        std::string out;
        out.reserve(in.size());
        for(std::size_t i = 0; i < in.size(); ++i)
        {
            if(in[i] == '%')
            {
                if(i + 3 <= in.size())
                {
                    int value = 0;
                    std::istringstream is(in.substr(i+1,2));
                    if(is >> std::hex >> value)
                    {
                        out += static_cast<char>(value);
                        i += 2;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else if(in[i] == '+')
            {
                out += ' ';
            }
            else
            {
                out += in[i];
            }
        }

        out_w = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(out);
        return true;
    }
}


//处理一个HTTP会话
class http_session : public std::enable_shared_from_this<http_session>
{
    //This queue is used for HTTP pipelining.
    class queue
    {
    public:
        //The type-erased, saved work item
        struct work
        {
            virtual ~work() = default;
            virtual void operator()() = 0;
        };

    public:
        explicit queue(http_session& self)
            :self_(self)
        {
            static_assert(limit > 0, "queue limit must be positive");
            items_.reserve(limit);
        }

        //Returns 'true' if we have reached the queue limit
        bool is_full() const
        {
            return items_.size() >= limit;
        }

        //Called when a message finishes sending
        //Returns 'true' if the caller should initiate a read
        bool on_write()
        {
            auto const was_full = is_full();
            items_.erase(items_.begin());
            if(!items_.empty())
            {
                (*items_.front())();
            }
            return was_full;
        }

        void operator()(boost::beast::http::message<false,boost::beast::http::basic_string_body<char8_t>,boost::beast::http::fields> msg);
        void operator()(boost::beast::http::message<false,boost::beast::http::empty_body,boost::beast::http::fields> msg);
        void operator()(boost::beast::http::message<false,boost::beast::http::file_body,boost::beast::http::fields> msg);
        //不使用boost::beast::file_body，因为他使用的boost::beast::file只针对2GB以下的文件
        void operator()(boost::beast::http::message<false,boost::beast::http::basic_file_body<file>,boost::beast::http::fields> msg);

    private:
        //This holds a work item
        template<bool isRequest, class Body, class Fields>
        struct work_impl : queue::work
        {
            http_session& self_;
            boost::beast::http::message<isRequest, Body, Fields> msg_;

            work_impl(
                    http_session& self,
                    boost::beast::http::message<isRequest, Body, Fields> msg
                    )
                :self_(self)
                , msg_(std::move(msg))
            {}

            void operator()()
            {
                boost::beast::http::async_write(
                            self_.stream(),
                            msg_,
                            boost::beast::bind_front_handler(
                                &http_session::on_write,
                                self_.shared_from_this(),
                                msg_.need_eof()
                                )
                            );
            }
        };

        enum
        {
            // Maximum number of responses we will queue
            limit = 8
        };

        http_session& self_;
        std::vector<std::unique_ptr<work>> items_;
    };

public:
    http_session(
            boost::asio::ip::tcp::socket&& socket
            , std::string const& server_root
            , std::mutex& mutex
            , KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool* threadpool
            )
        :stream_(std::move(socket))
        ,server_root_(server_root)
        ,queue_(*this)
        ,mutex_(mutex)
        ,threadpool_(threadpool)
    {}

    //开始会话过程
    void run()
    {
        do_read();
    }

    boost::beast::tcp_stream& stream(){return stream_;}

public:
    void on_write(bool close, boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
        {
            BOOST_LOG_TRIVIAL(error) << "write" << ":" << ec.message();
            return;
        }

        if(close)
        {
            //请求处理完后，返回close，表示服务器端是否要主动关闭连接
            return do_close();
        }

        //更新队列，即通知队列一个写操作完成
        if(queue_.on_write())
        {
            //如果队列已经满了，那么当队列减少一个时读下一个请求，可见每个http会话只有一个异步读
            do_read();
        }
    }

private:
    void do_read()
    {
        //为每个请求消息构造一个新的解析器
        parser_.emplace();

        //设置超时时间
        stream_.expires_after(std::chrono::seconds(20));

        //使用面向解析器接口读取请求信息
        boost::beast::http::async_read(
                    stream_,
                    buffer_,
                    *parser_,
                    boost::beast::bind_front_handler(&http_session::on_read, shared_from_this()));
    }

    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        //如果此时发现客户端套接字关闭了，那么服务器端套接字也进行关闭
        if(ec == boost::beast::http::error::end_of_stream)
        {
            return do_close();
        }

        if(ec)
        {
            BOOST_LOG_TRIVIAL(error) << "read" << ":" << ec.message();
            return;
        }

        //观察是否是一个WebSocket Upgrade
//        if(boost::beast::websocket::is_upgrade(parser_->get()))
//        {
//            //创建一个websocket会话，转移套接字和HTTP请求的所有权
//            std::make_shared<websocket_session>(
//                        stream_.release_socket())->do_accept(parser_->release());

//            return;
//        }

        //处理请求
        handle_request();

        //如果队列没有满，那么读取另一个请求，这里可以看出每个http会话只有一个异步读，所以即使多线程也不需要同步因为只有一个线程进入on_read，因为每个http会话中只有一个队列，所以同样也只有一个线程进入on_write
        if(! queue_.is_full())
            do_read();
    }

    void do_close()
    {
        //发送一个TCP shutdown
        boost::beast::error_code ec;
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);

        //这时，连接已标准方式关闭
    }

    template<typename Request>
    boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_bad_request_response(Request&& request, std::wstring const& why)
    {
        boost::property_tree::wptree tree;
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"400001"));
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.text"), why);

        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);

        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::bad_request, request.version()};
        response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"application/xml"));
        response_.keep_alive(request.keep_alive());
        response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(wsstream.str());
        response_.prepare_payload();
        return response_;
    }

    template<typename Request>
    boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_not_found_response(Request&& request, std::wstring const& target)
    {
        boost::property_tree::wptree tree;
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"404001"));
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"资源") + target + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide("没有找到"));

        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);

        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::not_found, request.version()};
        response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"application/xml"));
        response_.keep_alive(request.keep_alive());
        response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(wsstream.str());
        response_.prepare_payload();
        return response_;
    }

    template<typename Request>
    boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_server_error_response(Request&& request, std::wstring const& what)
    {
        boost::property_tree::wptree tree;
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"500001"));
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"发生错误：") + what);

        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);

        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::internal_server_error, request.version()};
        response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"application/xml"));
        response_.keep_alive(request.keep_alive());
        response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(wsstream.str());
        response_.prepare_payload();
        return response_;
    }

    template<typename Request>
    boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_unauthorized_response(Request&& request)
    {
        boost::property_tree::wptree tree;
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"401001"));
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"认证失败"));

        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);

        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::unauthorized, request.version()};
        response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"application/xml"));
        response_.set(boost::beast::http::field::www_authenticate, " Basic realm=\"kiwi.fast\"");
        response_.keep_alive(request.keep_alive());
        response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(wsstream.str());
        response_.prepare_payload();
        return response_;
    }

    template<typename Request>
    boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_ok_response(Request&& request)
    {
        boost::property_tree::wptree tree;
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"200001"));
        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"完成"));

        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);

        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::ok, request.version()};
        response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"application/xml"));
        response_.keep_alive(request.keep_alive());
        response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(wsstream.str());
        response_.prepare_payload();
        return response_;
    }

    void handle_request()
    {
        auto request = parser_->release();

        ////验证
        //确保是支持的HTTP-Method
        if(request.method() != boost::beast::http::verb::get
                && request.method() != boost::beast::http::verb::head
                && request.method() != boost::beast::http::verb::post
                && request.method() != boost::beast::http::verb::put
                && request.method() != boost::beast::http::verb::delete_)
        {
            //create_bad_request_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"未知HTTP-Method"));
            return queue_(create_bad_request_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"未知HTTP-Method")));
        }

        //请求路径必须是绝对路径，并且不能包含..
        if(request.target().empty()
                || request.target()[0] != '/'
                || request.target().find("..") != boost::beast::string_view::npos)
        {
            return queue_(create_bad_request_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"无效的REQUEST-TARGET")));
        }

        std::u8string request_target_decode;
        url_decode(std::string(request.target()), request_target_decode);

        //////////////////////////////////////////////////////////////////////////////

        if(request.method() == boost::beast::http::verb::post)
        {
            //站次数统计
            std::u8string query_condition_path_prefix = u8"/station_count/query_condition";
            std::u8string query_result_path_prefix = u8"/station_count/query_result";

            if(request_target_decode.substr(0, query_condition_path_prefix.size()) == query_condition_path_prefix)
            {
                std::lock_guard<std::mutex> lg(mutex_);

                try
                {
                    std::string body = boost::beast::buffers_to_string(request.body().data());

                    KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER station_count> query_condition_;
                    query_condition_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(body));

                    ////

                    query_condition_.handle([](int year, int month, int element, int interval, KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool* p_threadpool){

                        //获取db文件路径
                        std::vector<std::wstring> db_path;

                        sqlite3* pDB = nullptr;
                        int open_result = sqlite3_open_v2(reinterpret_cast<const char*>(u8R"(D:\OtherDocuments\HY3008\Output\sqlite\hysw_gyxx.db)"),
                                                          &pDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
                        if(open_result != SQLITE_OK)
                        {
                            return;//打开数据库连接失败
                        }

                        std::u8string sql = u8"SELECT qualityfile.path path FROM qualityfile WHERE 1=1 ";

                        boost::gregorian::date start_date(year, month, 1);
                        boost::gregorian::date end_date(start_date.end_of_month());
                        sql += u8" AND (qualityfile.ENDTIME>='";
                        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(boost::gregorian::to_sql_string(start_date));
                        sql += u8"' AND qualityfile.STARTTIME<='";
                        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(boost::gregorian::to_sql_string(end_date));
                        sql += u8"')";

                        sql += u8";";

                        sqlite3_stmt* sqlite_stmt = nullptr;
                        int prepare_result = sqlite3_prepare_v2(
                                    pDB
                                    , reinterpret_cast<const char*>(sql.c_str())
                                    , -1
                                    , &sqlite_stmt
                                    , NULL);
                        if(prepare_result != SQLITE_OK)
                        {
                            return;//查询语句错误
                        }

                        boost::filesystem::path db_root = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8R"(F:\DQ1028Data)");
                        db_root.remove_trailing_separator();

                        while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
                        {
                            const unsigned char* path = sqlite3_column_text(sqlite_stmt, 0);
                            db_path.push_back((db_root / KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(reinterpret_cast<const char8_t*>(path))).wstring());
                        }
                        sqlite3_finalize(sqlite_stmt);

                        if(pDB)
                        {
                            sqlite3_close_v2(pDB);
                            pDB = nullptr;
                        }

                        p_threadpool->post([](int year, int month, int element, int interval, std::vector<std::wstring> const& db_path){

                            std::vector<std::wstring> db_path_quotation;
                            std::transform(db_path.begin(), db_path.end(), std::back_inserter(db_path_quotation),[](std::wstring const& element){
                                return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"\"") + element + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"\"");
                            });
                            std::wstringstream db_path_quotation_wsstream;
                            std::copy(db_path_quotation.begin(), db_path_quotation.end(), std::ostream_iterator<std::wstring, wchar_t>(db_path_quotation_wsstream, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8" ").c_str()));

                            boost::process::child child_(
                                        boost::process::search_path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"kiwi.fast.batch_d"))
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"--year ") + std::to_string(year)
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"--month ") + std::to_string(month)
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"--element ") + std::to_string(element)
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"--interval ") + std::to_string(interval)
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"--sqlite_filepath ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(db_path_quotation_wsstream.str())
                                        , boost::process::std_out > boost::process::null
                                        , boost::process::std_err > boost::process::null);

                            child_.wait();
                            int result = child_.exit_code();
                            if(result != 0)
                            {
                                //日志记录没有执行成功
                            }

                        }, year, month, element, interval, db_path);
                    }, threadpool_);

                    ////

                    queue_(create_ok_response(request));
                }
                catch(...)
                {
                    return queue_(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"操作失败")));
                }
            }
            else if(request_target_decode.substr(0, query_result_path_prefix.size()) == query_result_path_prefix)
            {
                std::lock_guard<std::mutex> lg(mutex_);

                try
                {
                    std::string body = boost::beast::buffers_to_string(request.body().data());

                    KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER station_count> query_result_;
                    query_result_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(body));

                    ////

                    query_result_.handle([](int year, int month, int element, int interval, double lat, double lon, std::uint64_t count){

                        sqlite3* pDB = nullptr;
                        int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8R"(D:\OtherDocuments\HY3008\Output\sqlite\hysw_gyxx.db)").c_str(),
                                                          &pDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
                        if(open_result != SQLITE_OK)
                        {
                            return;//打开数据库连接失败
                        }

                        std::u8string sql = u8"INSERT INTO statistics_station_count(year,month,element,resolution,slat,slon,station_count) VALUES(";

                        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(year));
                        sql += u8",";
                        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(month));
                        sql += u8",";
                        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(element));
                        sql += u8",";
                        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(interval));
                        sql += u8",";
                        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lat));
                        sql += u8",";
                        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lon));
                        sql += u8",";
                        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(count));
                        sql += u8")";

                        sql += u8";";

                        sqlite3_stmt* sqlite_stmt = nullptr;
                        int prepare_result = sqlite3_prepare_v2(
                                    pDB
                                    , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(sql).c_str()
                                    , -1
                                    , &sqlite_stmt
                                    , NULL);
                        if(prepare_result != SQLITE_OK)
                        {
                            return;//语句错误
                        }

                        //执行该语句
                        sqlite3_step(sqlite_stmt);

                        sqlite3_finalize(sqlite_stmt);

                        if(pDB)
                        {
                            sqlite3_close_v2(pDB);
                            pDB = nullptr;
                        }
                    });

                    ////

                    queue_(create_ok_response(request));
                }
                catch(...)
                {
                    return queue_(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"操作失败")));
                }
            }
        }
        else if(request.method() == boost::beast::http::verb::put)
        {
            //更新
            if(request_target_decode.substr(0, 6) == u8"/dock")
            {
                std::lock_guard<std::mutex> lg(mutex_);

                try
                {
                    //...

                    queue_(create_ok_response(request));
                }
                catch(...)
                {
                    return queue_(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"操作失败")));
                }
            }
        }
        else if(request.method() == boost::beast::http::verb::delete_)
        {
            //删除ibox
            if(request_target_decode.substr(0, 6) == u8"/ibox/")
            {
                std::lock_guard<std::mutex> lg(mutex_);

                try
                {
                    //...

                    queue_(create_ok_response(request));
                }
                catch(...)
                {
                    return queue_(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"操作失败")));
                }
            }
        }
        else if(request.method() == boost::beast::http::verb::get
                || request.method() == boost::beast::http::verb::head)
        {
            //读取ibox
            if(request_target_decode.substr(0, 5) == u8"/ibox")
            {
                std::lock_guard<std::mutex> lg(mutex_);

                try
                {
                    boost::property_tree::wptree tree;
                    tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"200001"));
                    tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"完成"));

                    std::wstringstream wsstream;
                    boost::property_tree::write_xml(wsstream, tree);

                    boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::ok, request.version()};
                    response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
                    response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"application/xml"));
                    response_.keep_alive(request.keep_alive());
                    response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(wsstream.str());
                    response_.prepare_payload();
                    queue_(std::move(response_));
                }
                catch(...)
                {
                    return queue_(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"操作失败")));
                }
            }
        }



    }

private:
    boost::beast::tcp_stream stream_;//拥有套接字所有权
    boost::beast::flat_buffer buffer_;
    std::string server_root_;
    queue queue_;
    std::mutex& mutex_;
    KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool* threadpool_;

    boost::optional<boost::beast::http::request_parser<boost::beast::http::dynamic_body>> parser_;
};
