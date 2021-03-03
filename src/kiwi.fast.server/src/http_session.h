#pragma once

#include <kiwi.fast.server/detail/config.h>

#include <kiwi.fast.server/src/file.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <kiwi.fast.model/detail/station_count.h>
#include <kiwi.fast.model/detail/cengshen.h>
#include <kiwi.fast.model/detail/tongcun.h>
#include <kiwi.fast.model/detail/chongfu.h>
#include <kiwi.fast.model/detail/tongguo.h>

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

        out_w = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(out);
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
            :m_self(self)
        {
            static_assert(limit > 0, "queue limit must be positive");
            m_items.reserve(limit);
        }

        //Returns 'true' if we have reached the queue limit
        bool is_full() const
        {
            return m_items.size() >= limit;
        }

        //Called when a message finishes sending
        //Returns 'true' if the caller should initiate a read
        bool on_write()
        {
            auto const was_full = is_full();
            m_items.erase(m_items.begin());
            if(!m_items.empty())
            {
                (*m_items.front())();
            }
            return was_full;
        }

        // Called by the HTTP handler to send a response
        template<bool isRequest, class Body, class Fields>
        void operator()(boost::beast::http::message<isRequest, Body, Fields>&& msg)
        {
            // This holds a work item
            struct work_impl : work
            {
                http_session& m_self;
                boost::beast::http::message<isRequest, Body, Fields> m_msg;

                work_impl(
                            http_session& self,
                            boost::beast::http::message<isRequest, Body, Fields>&& msg)
                    :m_self(self)
                    ,m_msg(std::move(msg))
                {}

                void operator()()
                {
                    boost::beast::http::async_write(
                                m_self.m_stream,
                                m_msg,
                                boost::beast::bind_front_handler(
                                    &http_session::on_write,
                                    m_self.shared_from_this(),
                                    m_msg.need_eof()));
                }
            };

            // Allocate and store the work
            m_items.push_back(
                        boost::make_unique<work_impl>(m_self, std::move(msg)));

            // If there was no previous work, start this one
            if(m_items.size() == 1)
            {
                (*m_items.front())();
            }
        }

        enum
        {
            // Maximum number of responses we will queue
            limit = 8
        };

        http_session& m_self;
        std::vector<std::unique_ptr<work>> m_items;
    };

    class request_handler
    {
    public:
        explicit request_handler(http_session& self)
            :m_self(self)
        {}

        template<typename Request>
        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_bad_request_response(Request&& request, std::wstring const& why)
        {
            boost::property_tree::wptree tree;
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"400001"));
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.text"), why);

            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);

            boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::bad_request, request.version()};
            response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"application/xml"));
            response_.keep_alive(request.keep_alive());
            response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str());
            response_.prepare_payload();
            return response_;
        }

        template<typename Request>
        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_not_found_response(Request&& request, std::wstring const& target)
        {
            boost::property_tree::wptree tree;
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"404001"));
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"资源") + target + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>("没有找到"));

            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);

            boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::not_found, request.version()};
            response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"application/xml"));
            response_.keep_alive(request.keep_alive());
            response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str());
            response_.prepare_payload();
            return response_;
        }

        template<typename Request>
        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_server_error_response(Request&& request, std::wstring const& what)
        {
            boost::property_tree::wptree tree;
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"500001"));
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"发生错误：") + what);

            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);

            boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::internal_server_error, request.version()};
            response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"application/xml"));
            response_.keep_alive(request.keep_alive());
            response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str());
            response_.prepare_payload();
            return response_;
        }

        template<typename Request>
        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_unauthorized_response(Request&& request)
        {
            boost::property_tree::wptree tree;
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"401001"));
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"认证失败"));

            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);

            boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::unauthorized, request.version()};
            response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"application/xml"));
            response_.set(boost::beast::http::field::www_authenticate, " Basic realm=\"kiwi.fast\"");
            response_.keep_alive(request.keep_alive());
            response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str());
            response_.prepare_payload();
            return response_;
        }

        template<typename Request>
        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> create_ok_response(Request&& request)
        {
            boost::property_tree::wptree tree;
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"200001"));
            tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"完成"));

            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);

            boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::ok, request.version()};
            response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"application/xml"));
            response_.keep_alive(request.keep_alive());
            response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str());
            response_.prepare_payload();
            return response_;
        }

        template<class Body, class Fields>
        void operator()(boost::beast::http::request<Body, Fields>&& request)
        {
            ////验证
            //确保是支持的HTTP-Method
            if(request.method() != boost::beast::http::verb::get
                    && request.method() != boost::beast::http::verb::head
                    && request.method() != boost::beast::http::verb::post
                    && request.method() != boost::beast::http::verb::put
                    && request.method() != boost::beast::http::verb::delete_)
            {
                return m_self.m_queue(create_bad_request_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"未知HTTP-Method")));
            }

            //请求路径必须是绝对路径，并且不能包含..
            if(request.target().empty()
                    || request.target()[0] != '/'
                    || request.target().find("..") != boost::beast::string_view::npos)
            {
                return m_self.m_queue(create_bad_request_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"无效的REQUEST-TARGET")));
            }

            std::u8string request_target_decode;
            url_decode(std::string(request.target()), request_target_decode);

            if(request.method() == boost::beast::http::verb::post)
            {
                //站次数统计
                if(request_target_decode == u8"/station_count/query_condition")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER station_count> query_condition_;
                        query_condition_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_condition_.handle([](int year, int month, int element, int interval, KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu& threadpool){

                            //获取db文件路径
                            std::vector<std::wstring> db_path;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(reinterpret_cast<const char*>(u8R"(D:\HY3008\DQ1028Data\hysw_gyxx.db)"),
                                                              &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"SELECT qualityfile.path path FROM qualityfile WHERE 1=1 ";

                            boost::gregorian::date start_date(year, month, 1);
                            boost::gregorian::date end_date(start_date.end_of_month());
                            sql += u8" AND (qualityfile.ENDTIME>='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
                            sql += u8"' AND qualityfile.STARTTIME<='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
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

                            boost::filesystem::path db_root = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(D:\HY3008\DQ1028Data)");
                            db_root.remove_trailing_separator();

                            while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
                            {
                                const unsigned char* path = sqlite3_column_text(sqlite_stmt, 0);
                                db_path.push_back((db_root / (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(\)") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(reinterpret_cast<const char8_t*>(path)))).wstring());
                            }
                            sqlite3_finalize(sqlite_stmt);

                            if(pDB)
                            {
                                sqlite3_close_v2(pDB);
                                pDB = nullptr;
                            }

                            threadpool.post([](int year, int month, int element, int interval, std::vector<std::wstring> const& db_path){

                                try
                                {

                                    BOOST_LOG_TRIVIAL(error)
                                            << "/station_count/query_condition" << "---"
                                            << "year:" << year << " "
                                            << "month:" << month << " "
                                            << "element:" << element << " "
                                            << "interval:" << interval << " "
                                            << "file count:" << db_path.size() << " "
                                            << "cpu_mask:" << KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask();

                                    std::vector<std::wstring> db_path_quotation;
                                    std::transform(db_path.begin(), db_path.end(), std::back_inserter(db_path_quotation),[](std::wstring const& element){
                                        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"") + element + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"");
                                    });
                                    std::wstringstream db_path_quotation_wsstream;
                                    std::copy(db_path_quotation.begin(), db_path_quotation.end(), std::ostream_iterator<std::wstring, wchar_t>(db_path_quotation_wsstream, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8" ").c_str()));

    //                          	  boost::filesystem::path exe_path = boost::process::search_path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"kiwi.fast.batch_d"));
    //                          	  if(!boost::filesystem::exists(exe_path))
    //                          	  {
    //                          	      std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"执行文件不存在") << '\n';
    //                          	      return;
    //                          	  }

                                    std::string cmd = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\OtherDocuments\build-kiwi.fast-Desktop_Qt_MinGW_w64_64bit_MSYS2-Debug\bin\kiwi.fast.batch_d.exe)")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --cpu_mask ") + std::to_string(KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask())
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --name ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"station_count")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --year ") + std::to_string(year)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --month ") + std::to_string(month)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --element ") + std::to_string(element)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --interval ") + std::to_string(interval)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --sqlite_filepath ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(db_path_quotation_wsstream.str());

                                    boost::process::child child_(cmd);

                                    child_.wait();
                                    int result = child_.exit_code();
                                    if(result != 0)
                                    {
                                        BOOST_LOG_TRIVIAL(error)
                                                << "批处理进程非正常返回：" << result;
                                        return;
                                    }
                                }
                                catch(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << "[" << *boost::get_error_info<boost::throw_file>(ex) << "][" << *boost::get_error_info<boost::throw_line>(ex) << "]";
                                }
                                catch(std::exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << ex.what();
                                }
                                catch(...)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程未知异常";
                                }

                            }, year, month, element, interval, db_path);
                        }, std::ref(m_self.m_global_threadpool));

                        ////

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }
                else if(request_target_decode == u8"/station_count/query_result")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER station_count> query_result_;
                        query_result_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_result_.handle([&](int year, int month, int element, int interval, double lat, double lon, std::uint64_t count){

                            BOOST_LOG_TRIVIAL(error)
                                        << "/station_count/query_result" << "---"
                                        << "year:" << year << " "
                                        << "month:" << month << " "
                                        << "element:" << element << " "
                                        << "interval:" << interval << " "
                                        << "lat:" << lat << " "
                                        << "lon:" << lon << " "
                                        << "count:" << count;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(u8R"(D:\HY3008\DQ1028Data\hysw_gyxx.db)").c_str(),
                                                              &pDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"INSERT INTO statistics_station_count(year,month,element,resolution,slat,slon,station_count) VALUES(";

                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(year));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(month));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(element));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(interval));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(count));
                            sql += u8")";

                            sql += u8";";

                            sqlite3_stmt* sqlite_stmt = nullptr;
                            int prepare_result = sqlite3_prepare_v2(
                                        pDB
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(sql).c_str()
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

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }
                //层深统计
                else if(request_target_decode == u8"/cengshen/query_condition")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER cengshen> query_condition_;
                        query_condition_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_condition_.handle([](int year, int month, int element, int interval, KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu& threadpool){

                            //获取db文件路径
                            std::vector<std::wstring> db_path;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(reinterpret_cast<const char*>(u8R"(E:\hysw_gyxx.db)"),
                                                              &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"SELECT qualityfile.path path FROM qualityfile WHERE 1=1 ";

                            boost::gregorian::date start_date(year, month, 1);
                            boost::gregorian::date end_date(start_date.end_of_month());
                            sql += u8" AND (qualityfile.ENDTIME>='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
                            sql += u8"' AND qualityfile.STARTTIME<='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
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

                            boost::filesystem::path db_root = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(E:\DQ1028Data)");
                            db_root.remove_trailing_separator();

                            while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
                            {
                                const unsigned char* path = sqlite3_column_text(sqlite_stmt, 0);
                                db_path.push_back((db_root / (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(\)") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(reinterpret_cast<const char8_t*>(path)))).wstring());
                            }
                            sqlite3_finalize(sqlite_stmt);

                            if(pDB)
                            {
                                sqlite3_close_v2(pDB);
                                pDB = nullptr;
                            }

                            threadpool.post([](int year, int month, int element, int interval, std::vector<std::wstring> const& db_path){

                                try
                                {
                                BOOST_LOG_TRIVIAL(error)
                                        << "/cengshen/query_condition" << "---"
                                        << "year:" << year << " "
                                        << "month:" << month << " "
                                        << "element:" << element << " "
                                        << "interval:" << interval << " "
                                        << "file count:" << db_path.size() << " "
                                        << "cpu_mask:" << KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask();

                                std::vector<std::wstring> db_path_quotation;
                                std::transform(db_path.begin(), db_path.end(), std::back_inserter(db_path_quotation),[](std::wstring const& element){
                                    return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"") + element + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"");
                                });
                                std::wstringstream db_path_quotation_wsstream;
                                std::copy(db_path_quotation.begin(), db_path_quotation.end(), std::ostream_iterator<std::wstring, wchar_t>(db_path_quotation_wsstream, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8" ").c_str()));

    //                            boost::filesystem::path exe_path = boost::process::search_path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"kiwi.fast.batch_d"));
    //                            if(!boost::filesystem::exists(exe_path))
    //                            {
    //                                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"执行文件不存在") << '\n';
    //                                return;
    //                            }

                                boost::process::child child_(
                                            //必须完整路径
                                            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\OtherDocuments\build-kiwi.fast-Desktop_Qt_MinGW_w64_64bit_MSYS2-Debug\bin\kiwi.fast.batch_d.exe)")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --cpu_mask ") + std::to_string(KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask())
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --name ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"cengshen")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --year ") + std::to_string(year)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --month ") + std::to_string(month)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --element ") + std::to_string(element)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --interval ") + std::to_string(interval)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --sqlite_filepath ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(db_path_quotation_wsstream.str()));

                                child_.wait();
                                int result = child_.exit_code();
                                if(result != 0)
                                {
                                    return;
                                }
                                }
                                catch(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << "[" << *boost::get_error_info<boost::throw_file>(ex) << "][" << *boost::get_error_info<boost::throw_line>(ex) << "]";
                                }
                                catch(std::exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << ex.what();
                                }
                                catch(...)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程未知异常";
                                }


                            }, year, month, element, interval, db_path);
                        }, std::ref(m_self.m_global_threadpool));

                        ////

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }
                else if(request_target_decode == u8"/cengshen/query_result")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER cengshen> query_result_;
                        query_result_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_result_.handle([&](int year, int month, int element, int interval, double lat, double lon, std::uint64_t all
                                             ,std::uint64_t d0
                                             ,std::uint64_t d5
                                             ,std::uint64_t d10
                                             ,std::uint64_t d15
                                             ,std::uint64_t d20
                                             ,std::uint64_t d25
                                             ,std::uint64_t d30
                                             ,std::uint64_t d35
                                             ,std::uint64_t d40
                                             ,std::uint64_t d45
                                             ,std::uint64_t d50
                                             ,std::uint64_t d55
                                             ,std::uint64_t d60
                                             ,std::uint64_t d65
                                             ,std::uint64_t d70
                                             ,std::uint64_t d75
                                             ,std::uint64_t d80
                                             ,std::uint64_t d85
                                             ,std::uint64_t d90
                                             ,std::uint64_t d95
                                             ,std::uint64_t d100
                                             ,std::uint64_t d125
                                             ,std::uint64_t d150
                                             ,std::uint64_t d175
                                             ,std::uint64_t d200
                                             ,std::uint64_t d225
                                             ,std::uint64_t d250
                                             ,std::uint64_t d275
                                             ,std::uint64_t d300
                                             ,std::uint64_t d325
                                             ,std::uint64_t d350
                                             ,std::uint64_t d375
                                             ,std::uint64_t d400
                                             ,std::uint64_t d425
                                             ,std::uint64_t d450
                                             ,std::uint64_t d475
                                             ,std::uint64_t d500
                                             ,std::uint64_t d550
                                             ,std::uint64_t d600
                                             ,std::uint64_t d650
                                             ,std::uint64_t d700
                                             ,std::uint64_t d750
                                             ,std::uint64_t d800
                                             ,std::uint64_t d850
                                             ,std::uint64_t d900
                                             ,std::uint64_t d950
                                             ,std::uint64_t d1000
                                             ,std::uint64_t d1050
                                             ,std::uint64_t d1100
                                             ,std::uint64_t d1150
                                             ,std::uint64_t d1200
                                             ,std::uint64_t d1250
                                             ,std::uint64_t d1300
                                             ,std::uint64_t d1350
                                             ,std::uint64_t d1400
                                             ,std::uint64_t d1450
                                             ,std::uint64_t d1500
                                             ,std::uint64_t d1550
                                             ,std::uint64_t d1600
                                             ,std::uint64_t d1650
                                             ,std::uint64_t d1700
                                             ,std::uint64_t d1750
                                             ,std::uint64_t d1800
                                             ,std::uint64_t d1850
                                             ,std::uint64_t d1900
                                             ,std::uint64_t d1950
                                             ,std::uint64_t d2000
                                             ,std::uint64_t d2100
                                             ,std::uint64_t d2200
                                             ,std::uint64_t d2300
                                             ,std::uint64_t d2400
                                             ,std::uint64_t d2500
                                             ,std::uint64_t d2600
                                             ,std::uint64_t d2700
                                             ,std::uint64_t d2800
                                             ,std::uint64_t d2900
                                             ,std::uint64_t d3000
                                             ,std::uint64_t d3100
                                             ,std::uint64_t d3200
                                             ,std::uint64_t d3300
                                             ,std::uint64_t d3400
                                             ,std::uint64_t d3500
                                             ,std::uint64_t d3600
                                             ,std::uint64_t d3700
                                             ,std::uint64_t d3800
                                             ,std::uint64_t d3900
                                             ,std::uint64_t d4000
                                             ,std::uint64_t d4100
                                             ,std::uint64_t d4200
                                             ,std::uint64_t d4300
                                             ,std::uint64_t d4400
                                             ,std::uint64_t d4500
                                             ,std::uint64_t d4600
                                             ,std::uint64_t d4700
                                             ,std::uint64_t d4800
                                             ,std::uint64_t d4900
                                             ,std::uint64_t d5000
                                             ,std::uint64_t d5100
                                             ,std::uint64_t d5200
                                             ,std::uint64_t d5300
                                             ,std::uint64_t d5400
                                             ,std::uint64_t d5500
                                             ,std::uint64_t d5600
                                             ,std::uint64_t d5700
                                             ,std::uint64_t d5800
                                             ,std::uint64_t d5900
                                             ,std::uint64_t d6000
                                             ,std::uint64_t d6100
                                             ,std::uint64_t d6200
                                             ,std::uint64_t d6300
                                             ,std::uint64_t d6400
                                             ,std::uint64_t d6500
                                             ,std::uint64_t d6600
                                             ,std::uint64_t d6700
                                             ,std::uint64_t d6800
                                             ,std::uint64_t d6900
                                             ,std::uint64_t d7000
                                             ,std::uint64_t d7100
                                             ,std::uint64_t d7200
                                             ,std::uint64_t d7300
                                             ,std::uint64_t d7400
                                             ,std::uint64_t d7500
                                             ,std::uint64_t d7600
                                             ,std::uint64_t d7700
                                             ,std::uint64_t d7800
                                             ,std::uint64_t d7900
                                             ,std::uint64_t d8000
                                             ,std::uint64_t d8100
                                             ,std::uint64_t d8200
                                             ,std::uint64_t d8300
                                             ,std::uint64_t d8400
                                             ,std::uint64_t d8500
                                             ,std::uint64_t d8600
                                             ,std::uint64_t d8700
                                             ,std::uint64_t d8800
                                             ,std::uint64_t d8900
                                             ,std::uint64_t d9000
                                             ){

                            BOOST_LOG_TRIVIAL(error)
                                        << "/cengshen/query_result" << "---"
                                        << "year:" << year << " "
                                        << "month:" << month << " "
                                        << "element:" << element << " "
                                        << "interval:" << interval << " "
                                        << "lat:" << lat << " "
                                        << "lon:" << lon << " "
                                        << "all:" << all << " "
                                        << "d0:" << d0 << " "
                                        << "d5:" << d5 << " "
                                        << "d10:" << d10 << " "
                                        << "d15:" << d15 << " "
                                        << "d20:" << d20 << " "
                                        << "d25:" << d25 << " "
                                        << "d30:" << d30 << " "
                                        << "d35:" << d35 << " "
                                        << "d40:" << d40 << " "
                                        << "d45:" << d45 << " "
                                        << "d50:" << d50 << " "
                                        << "d55:" << d55 << " "
                                        << "d60:" << d60 << " "
                                        << "d65:" << d65 << " "
                                        << "d70:" << d70 << " "
                                        << "d75:" << d75 << " "
                                        << "d80:" << d80 << " "
                                        << "d85:" << d85 << " "
                                        << "d90:" << d90 << " "
                                        << "d95:" << d95 << " "
                                        << "d100:" << d100 << " "
                                        << "d125:" << d125 << " "
                                        << "d150:" << d150 << " "
                                        << "d175:" << d175 << " "
                                        << "d200:" << d200 << " "
                                        << "d225:" << d225 << " "
                                        << "d250:" << d250 << " "
                                        << "d275:" << d275 << " "
                                        << "d300:" << d300 << " "
                                        << "d325:" << d325 << " "
                                        << "d350:" << d350 << " "
                                        << "d375:" << d375 << " "
                                        << "d400:" << d400 << " "
                                        << "d425:" << d425 << " "
                                        << "d450:" << d450 << " "
                                        << "d475:" << d475 << " "
                                        << "d500:" << d500 << " "
                                        << "d550:" << d550 << " "
                                        << "d600:" << d600 << " "
                                        << "d650:" << d650 << " "
                                        << "d700:" << d700 << " "
                                        << "d750:" << d750 << " "
                                        << "d800:" << d800 << " "
                                        << "d850:" << d850 << " "
                                        << "d900:" << d900 << " "
                                        << "d950:" << d950 << " "
                                        << "d1000:" << d1000 << " "
                                        << "d1050:" << d1050 << " "
                                        << "d1100:" << d1100 << " "
                                        << "d1150:" << d1150 << " "
                                        << "d1200:" << d1200 << " "
                                        << "d1250:" << d1250 << " "
                                        << "d1300:" << d1300 << " "
                                        << "d1350:" << d1350 << " "
                                        << "d1400:" << d1400 << " "
                                        << "d1450:" << d1450 << " "
                                        << "d1500:" << d1500 << " "
                                        << "d1550:" << d1550 << " "
                                        << "d1600:" << d1600 << " "
                                        << "d1650:" << d1650 << " "
                                        << "d1700:" << d1700 << " "
                                        << "d1750:" << d1750 << " "
                                        << "d1800:" << d1800 << " "
                                        << "d1850:" << d1850 << " "
                                        << "d1900:" << d1900 << " "
                                        << "d1950:" << d1950 << " "
                                        << "d2000:" << d2000 << " "
                                        << "d2100:" << d2100 << " "
                                        << "d2200:" << d2200 << " "
                                        << "d2300:" << d2300 << " "
                                        << "d2400:" << d2400 << " "
                                        << "d2500:" << d2500 << " "
                                        << "d2600:" << d2600 << " "
                                        << "d2700:" << d2700 << " "
                                        << "d2800:" << d2800 << " "
                                        << "d2900:" << d2900 << " "
                                        << "d3000:" << d3000 << " "
                                        << "d3100:" << d3100 << " "
                                        << "d3200:" << d3200 << " "
                                        << "d3300:" << d3300 << " "
                                        << "d3400:" << d3400 << " "
                                        << "d3500:" << d3500 << " "
                                        << "d3600:" << d3600 << " "
                                        << "d3700:" << d3700 << " "
                                        << "d3800:" << d3800 << " "
                                        << "d3900:" << d3900 << " "
                                        << "d4000:" << d4000 << " "
                                        << "d4100:" << d4100 << " "
                                        << "d4200:" << d4200 << " "
                                        << "d4300:" << d4300 << " "
                                        << "d4400:" << d4400 << " "
                                        << "d4500:" << d4500 << " "
                                        << "d4600:" << d4600 << " "
                                        << "d4700:" << d4700 << " "
                                        << "d4800:" << d4800 << " "
                                        << "d4900:" << d4900 << " "
                                        << "d5000:" << d5000 << " "
                                        << "d5100:" << d5100 << " "
                                        << "d5200:" << d5200 << " "
                                        << "d5300:" << d5300 << " "
                                        << "d5400:" << d5400 << " "
                                        << "d5500:" << d5500 << " "
                                        << "d5600:" << d5600 << " "
                                        << "d5700:" << d5700 << " "
                                        << "d5800:" << d5800 << " "
                                        << "d5900:" << d5900 << " "
                                        << "d6000:" << d6000 << " "
                                        << "d6100:" << d6100 << " "
                                        << "d6200:" << d6200 << " "
                                        << "d6300:" << d6300 << " "
                                        << "d6400:" << d6400 << " "
                                        << "d6500:" << d6500 << " "
                                        << "d6600:" << d6600 << " "
                                        << "d6700:" << d6700 << " "
                                        << "d6800:" << d6800 << " "
                                        << "d6900:" << d6900 << " "
                                        << "d7000:" << d7000 << " "
                                        << "d7100:" << d7100 << " "
                                        << "d7200:" << d7200 << " "
                                        << "d7300:" << d7300 << " "
                                        << "d7400:" << d7400 << " "
                                        << "d7500:" << d7500 << " "
                                        << "d7600:" << d7600 << " "
                                        << "d7700:" << d7700 << " "
                                        << "d7800:" << d7800 << " "
                                        << "d7900:" << d7900 << " "
                                        << "d8000:" << d8000 << " "
                                        << "d8100:" << d8100 << " "
                                        << "d8200:" << d8200 << " "
                                        << "d8300:" << d8300 << " "
                                        << "d8400:" << d8400 << " "
                                        << "d8500:" << d8500 << " "
                                        << "d8600:" << d8600 << " "
                                        << "d8700:" << d8700 << " "
                                        << "d8800:" << d8800 << " "
                                        << "d8900:" << d8900 << " "
                                        << "d9000:" << d9000
                                           ;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(u8R"(E:\hysw_gyxx.db)").c_str(),
                                                              &pDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"INSERT INTO statistics_cengshen(year,month,element,resolution,slat,slon,all";
                            sql += u8",cengshen0";
                            sql += u8",cengshen5";
                            sql += u8",cengshen10";
                            sql += u8",cengshen15";
                            sql += u8",cengshen20";
                            sql += u8",cengshen25";
                            sql += u8",cengshen30";
                            sql += u8",cengshen35";
                            sql += u8",cengshen40";
                            sql += u8",cengshen45";
                            sql += u8",cengshen50";
                            sql += u8",cengshen55";
                            sql += u8",cengshen60";
                            sql += u8",cengshen65";
                            sql += u8",cengshen70";
                            sql += u8",cengshen75";
                            sql += u8",cengshen80";
                            sql += u8",cengshen85";
                            sql += u8",cengshen90";
                            sql += u8",cengshen95";
                            sql += u8",cengshen100";
                            sql += u8",cengshen125";
                            sql += u8",cengshen150";
                            sql += u8",cengshen175";
                            sql += u8",cengshen200";
                            sql += u8",cengshen225";
                            sql += u8",cengshen250";
                            sql += u8",cengshen275";
                            sql += u8",cengshen300";
                            sql += u8",cengshen325";
                            sql += u8",cengshen350";
                            sql += u8",cengshen375";
                            sql += u8",cengshen400";
                            sql += u8",cengshen425";
                            sql += u8",cengshen450";
                            sql += u8",cengshen475";
                            sql += u8",cengshen500";
                            sql += u8",cengshen550";
                            sql += u8",cengshen600";
                            sql += u8",cengshen650";
                            sql += u8",cengshen700";
                            sql += u8",cengshen750";
                            sql += u8",cengshen800";
                            sql += u8",cengshen850";
                            sql += u8",cengshen900";
                            sql += u8",cengshen950";
                            sql += u8",cengshen1000";
                            sql += u8",cengshen1050";
                            sql += u8",cengshen1100";
                            sql += u8",cengshen1150";
                            sql += u8",cengshen1200";
                            sql += u8",cengshen1250";
                            sql += u8",cengshen1300";
                            sql += u8",cengshen1350";
                            sql += u8",cengshen1400";
                            sql += u8",cengshen1450";
                            sql += u8",cengshen1500";
                            sql += u8",cengshen1550";
                            sql += u8",cengshen1600";
                            sql += u8",cengshen1650";
                            sql += u8",cengshen1700";
                            sql += u8",cengshen1750";
                            sql += u8",cengshen1800";
                            sql += u8",cengshen1850";
                            sql += u8",cengshen1900";
                            sql += u8",cengshen1950";
                            sql += u8",cengshen2000";
                            sql += u8",cengshen2100";
                            sql += u8",cengshen2200";
                            sql += u8",cengshen2300";
                            sql += u8",cengshen2400";
                            sql += u8",cengshen2500";
                            sql += u8",cengshen2600";
                            sql += u8",cengshen2700";
                            sql += u8",cengshen2800";
                            sql += u8",cengshen2900";
                            sql += u8",cengshen3000";
                            sql += u8",cengshen3100";
                            sql += u8",cengshen3200";
                            sql += u8",cengshen3300";
                            sql += u8",cengshen3400";
                            sql += u8",cengshen3500";
                            sql += u8",cengshen3600";
                            sql += u8",cengshen3700";
                            sql += u8",cengshen3800";
                            sql += u8",cengshen3900";
                            sql += u8",cengshen4000";
                            sql += u8",cengshen4100";
                            sql += u8",cengshen4200";
                            sql += u8",cengshen4300";
                            sql += u8",cengshen4400";
                            sql += u8",cengshen4500";
                            sql += u8",cengshen4600";
                            sql += u8",cengshen4700";
                            sql += u8",cengshen4800";
                            sql += u8",cengshen4900";
                            sql += u8",cengshen5000";
                            sql += u8",cengshen5100";
                            sql += u8",cengshen5200";
                            sql += u8",cengshen5300";
                            sql += u8",cengshen5400";
                            sql += u8",cengshen5500";
                            sql += u8",cengshen5600";
                            sql += u8",cengshen5700";
                            sql += u8",cengshen5800";
                            sql += u8",cengshen5900";
                            sql += u8",cengshen6000";
                            sql += u8",cengshen6100";
                            sql += u8",cengshen6200";
                            sql += u8",cengshen6300";
                            sql += u8",cengshen6400";
                            sql += u8",cengshen6500";
                            sql += u8",cengshen6600";
                            sql += u8",cengshen6700";
                            sql += u8",cengshen6800";
                            sql += u8",cengshen6900";
                            sql += u8",cengshen7000";
                            sql += u8",cengshen7100";
                            sql += u8",cengshen7200";
                            sql += u8",cengshen7300";
                            sql += u8",cengshen7400";
                            sql += u8",cengshen7500";
                            sql += u8",cengshen7600";
                            sql += u8",cengshen7700";
                            sql += u8",cengshen7800";
                            sql += u8",cengshen7900";
                            sql += u8",cengshen8000";
                            sql += u8",cengshen8100";
                            sql += u8",cengshen8200";
                            sql += u8",cengshen8300";
                            sql += u8",cengshen8400";
                            sql += u8",cengshen8500";
                            sql += u8",cengshen8600";
                            sql += u8",cengshen8700";
                            sql += u8",cengshen8800";
                            sql += u8",cengshen8900";
                            sql += u8",cengshen9000";
                            sql += u8") VALUES(";

                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(year));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(month));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(element));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(interval));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(all));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d0));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d10));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d15));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d20));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d25));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d30));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d35));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d40));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d45));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d50));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d55));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d60));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d65));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d70));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d75));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d80));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d85));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d90));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d95));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d100));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d125));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d150));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d175));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d200));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d225));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d250));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d275));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d300));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d325));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d350));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d375));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d400));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d425));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d450));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d475));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d500));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d550));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d600));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d650));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d700));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d750));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d800));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d850));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d900));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d950));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1000));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1050));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1100));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1150));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1200));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1250));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1300));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1350));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1400));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1450));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1500));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1550));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1600));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1650));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1700));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1750));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1800));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1850));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1900));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d1950));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2000));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2100));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2200));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2300));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2400));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2500));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2600));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2700));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2800));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d2900));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3000));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3100));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3200));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3300));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3400));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3500));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3600));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3700));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3800));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d3900));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4000));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4100));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4200));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4300));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4400));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4500));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4600));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4700));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4800));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d4900));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5000));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5100));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5200));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5300));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5400));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5500));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5600));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5700));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5800));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d5900));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6000));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6100));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6200));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6300));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6400));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6500));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6600));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6700));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6800));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d6900));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7000));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7100));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7200));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7300));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7400));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7500));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7600));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7700));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7800));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d7900));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8000));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8100));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8200));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8300));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8400));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8500));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8600));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8700));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8800));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d8900));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(d9000));
                            sql += u8")";

                            sql += u8";";

                            sqlite3_stmt* sqlite_stmt = nullptr;
                            int prepare_result = sqlite3_prepare_v2(
                                        pDB
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(sql).c_str()
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

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }
                //同存统计
                else if(request_target_decode == u8"/tongcun/query_condition")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongcun> query_condition_;
                        query_condition_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_condition_.handle([](int year, int month, int interval, KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu& threadpool){


                            //获取db文件路径
                            std::vector<std::wstring> db_path;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(reinterpret_cast<const char*>(u8R"(X:\tongcun\hysw_gyxx.db)"),
                                                              &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"SELECT qualityfile.path path FROM qualityfile WHERE 1=1 ";

                            boost::gregorian::date start_date(year, month, 1);
                            boost::gregorian::date end_date(start_date.end_of_month());
                            sql += u8" AND (qualityfile.ENDTIME>='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
                            sql += u8"' AND qualityfile.STARTTIME<='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
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

                            boost::filesystem::path db_root = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(X:\)");
                            db_root.remove_trailing_separator();

                            while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
                            {
                                const unsigned char* path = sqlite3_column_text(sqlite_stmt, 0);
                                db_path.push_back((db_root / (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(\)") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(reinterpret_cast<const char8_t*>(path)))).wstring());
                            }
                            sqlite3_finalize(sqlite_stmt);

                            if(pDB)
                            {
                                sqlite3_close_v2(pDB);
                                pDB = nullptr;
                            }

                            threadpool.post([](int year, int month, int interval, std::vector<std::wstring> const& db_path){

                                try
                                {


                                BOOST_LOG_TRIVIAL(error)
                                        << "/tongcun/query_condition" << "---"
                                        << "year:" << year << " "
                                        << "month:" << month << " "
                                        << "interval:" << interval << " "
                                        << "file count:" << db_path.size() << " "
                                        << "cpu_mask:" << KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask();

                                std::vector<std::wstring> db_path_quotation;
                                std::transform(db_path.begin(), db_path.end(), std::back_inserter(db_path_quotation),[](std::wstring const& element){
                                    return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"") + element + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"");
                                });
                                std::wstringstream db_path_quotation_wsstream;
                                std::copy(db_path_quotation.begin(), db_path_quotation.end(), std::ostream_iterator<std::wstring, wchar_t>(db_path_quotation_wsstream, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8" ").c_str()));

    //                            boost::filesystem::path exe_path = boost::process::search_path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"kiwi.fast.batch_d"));
    //                            if(!boost::filesystem::exists(exe_path))
    //                            {
    //                                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"执行文件不存在") << '\n';
    //                                return;
    //                            }

                                boost::process::child child_(
                                            //必须完整路径
                                            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\OtherDocuments\build-kiwi.fast-Desktop_Qt_MinGW_w64_64bit_MSYS2-Debug\bin\kiwi.fast.batch_d.exe)")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --cpu_mask ") + std::to_string(KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask())
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --name ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"tongcun")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --year ") + std::to_string(year)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --month ") + std::to_string(month)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --element ") + std::to_string(0)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --interval ") + std::to_string(interval)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --sqlite_filepath ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(db_path_quotation_wsstream.str()));

                                child_.wait();
                                int result = child_.exit_code();
                                if(result != 0)
                                {
                                    return;
                                }
                                }
                                catch(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << "[" << *boost::get_error_info<boost::throw_file>(ex) << "][" << *boost::get_error_info<boost::throw_line>(ex) << "]";
                                }
                                catch(std::exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << ex.what();
                                }
                                catch(...)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程未知异常";
                                }

                            }, year, month, interval, db_path);
                        }, std::ref(m_self.m_global_threadpool));

                        ////

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }
                else if(request_target_decode == u8"/tongcun/query_result")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongcun> query_result_;
                        query_result_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_result_.handle([&](int year, int month, int interval, double lat, double lon, std::uint64_t tongcun_count, std::uint64_t total_count){

                            BOOST_LOG_TRIVIAL(error)
                                        << "/tongcun/query_result" << "---"
                                        << "year:" << year << " "
                                        << "month:" << month << " "
                                        << "interval:" << interval << " "
                                        << "lat:" << lat << " "
                                        << "lon:" << lon << " "
                                        << "tongcun_count:" << tongcun_count << " "
                                        << "total_count:" << total_count
                                           ;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(u8R"(X:\tongcun\hysw_gyxx.db)").c_str(),
                                                              &pDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"INSERT INTO statistics_tongcun(year,month,resolution,slat,slon,tongcun_count,total_count) VALUES(";

                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(year));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(month));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(interval));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(tongcun_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(total_count));
                            sql += u8")";

                            sql += u8";";

                            sqlite3_stmt* sqlite_stmt = nullptr;
                            int prepare_result = sqlite3_prepare_v2(
                                        pDB
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(sql).c_str()
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

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }
                //重复率统计
                else if(request_target_decode == u8"/chongfu/query_condition")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER chongfu> query_condition_;
                        query_condition_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_condition_.handle([](int year, int month, int element, int interval, KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu& threadpool){


                            //获取db文件路径
                            std::vector<std::wstring> db_path;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(reinterpret_cast<const char*>(u8R"(X:\tongcun\hysw_gyxx.db)"),
                                                              &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"SELECT qualityfile.path path FROM qualityfile WHERE 1=1 ";

                            boost::gregorian::date start_date(year, month, 1);
                            boost::gregorian::date end_date(start_date.end_of_month());
                            sql += u8" AND (qualityfile.ENDTIME>='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
                            sql += u8"' AND qualityfile.STARTTIME<='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
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

                            boost::filesystem::path db_root = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(X:\)");
                            db_root.remove_trailing_separator();

                            while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
                            {
                                const unsigned char* path = sqlite3_column_text(sqlite_stmt, 0);
                                db_path.push_back((db_root / (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(\)") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(reinterpret_cast<const char8_t*>(path)))).wstring());
                            }
                            sqlite3_finalize(sqlite_stmt);

                            if(pDB)
                            {
                                sqlite3_close_v2(pDB);
                                pDB = nullptr;
                            }

                            threadpool.post([](int year, int month, int element, int interval, std::vector<std::wstring> const& db_path){

                                try
                                {


                                BOOST_LOG_TRIVIAL(error)
                                        << "/chongfu/query_condition" << "---"
                                        << "year:" << year << " "
                                        << "month:" << month << " "
                                        << "element:" << element << " "
                                        << "interval:" << interval << " "
                                        << "file count:" << db_path.size() << " "
                                        << "cpu_mask:" << KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask();

                                std::vector<std::wstring> db_path_quotation;
                                std::transform(db_path.begin(), db_path.end(), std::back_inserter(db_path_quotation),[](std::wstring const& element){
                                    return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"") + element + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"");
                                });
                                std::wstringstream db_path_quotation_wsstream;
                                std::copy(db_path_quotation.begin(), db_path_quotation.end(), std::ostream_iterator<std::wstring, wchar_t>(db_path_quotation_wsstream, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8" ").c_str()));

    //                            boost::filesystem::path exe_path = boost::process::search_path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"kiwi.fast.batch_d"));
    //                            if(!boost::filesystem::exists(exe_path))
    //                            {
    //                                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"执行文件不存在") << '\n';
    //                                return;
    //                            }

                                boost::process::child child_(
                                            //必须完整路径
                                            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\OtherDocuments\build-kiwi.fast-Desktop_Qt_MinGW_w64_64bit_MSYS2-Debug\bin\kiwi.fast.batch_d.exe)")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --cpu_mask ") + std::to_string(KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask())
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --name ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"chongfu")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --year ") + std::to_string(year)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --month ") + std::to_string(month)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --element ") + std::to_string(element)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --interval ") + std::to_string(interval)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --sqlite_filepath ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(db_path_quotation_wsstream.str()));

                                child_.wait();
                                int result = child_.exit_code();
                                if(result != 0)
                                {
                                    return;
                                }
                                }
                                catch(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << "[" << *boost::get_error_info<boost::throw_file>(ex) << "][" << *boost::get_error_info<boost::throw_line>(ex) << "]";
                                }
                                catch(std::exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << ex.what();
                                }
                                catch(...)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程未知异常";
                                }

                            }, year, month, element, interval, db_path);
                        }, std::ref(m_self.m_global_threadpool));

                        ////

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }
                else if(request_target_decode == u8"/chongfu/query_result")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER chongfu> query_result_;
                        query_result_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_result_.handle([&](int year, int month, int element, int interval, double lat, double lon, std::uint64_t chongfu_count, std::uint64_t total_count){

                            BOOST_LOG_TRIVIAL(error)
                                        << "/chongfu/query_result" << "---"
                                        << "year:" << year << " "
                                        << "month:" << month << " "
                                        << "element:" << element << " "
                                        << "interval:" << interval << " "
                                        << "lat:" << lat << " "
                                        << "lon:" << lon << " "
                                        << "chongfu_count:" << chongfu_count << " "
                                        << "total_count:" << total_count
                                           ;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(u8R"(X:\tongcun\hysw_gyxx.db)").c_str(),
                                                              &pDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"INSERT INTO statistics_chongfu(year,month,element,resolution,slat,slon,chongfu_count,total_count) VALUES(";

                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(year));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(month));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(element));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(interval));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(chongfu_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(total_count));
                            sql += u8")";

                            sql += u8";";

                            sqlite3_stmt* sqlite_stmt = nullptr;
                            int prepare_result = sqlite3_prepare_v2(
                                        pDB
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(sql).c_str()
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

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }
                //通过率
                if(request_target_decode == u8"/tongguo/query_condition")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongguo> query_condition_;
                        query_condition_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_condition_.handle([](int year, int month, int element, int interval, KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu& threadpool){

                            //获取db文件路径
                            std::vector<std::wstring> db_path;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(reinterpret_cast<const char*>(u8R"(X:\tongcun\hysw_gyxx.db)"),
                                                              &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"SELECT qualityfile.path path FROM qualityfile WHERE 1=1 ";

                            boost::gregorian::date start_date(year, month, 1);
                            boost::gregorian::date end_date(start_date.end_of_month());
                            sql += u8" AND (qualityfile.ENDTIME>='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
                            sql += u8"' AND qualityfile.STARTTIME<='";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
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

                            boost::filesystem::path db_root = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(X:\)");
                            db_root.remove_trailing_separator();

                            while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
                            {
                                const unsigned char* path = sqlite3_column_text(sqlite_stmt, 0);
                                db_path.push_back((db_root / (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8R"(\)") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(reinterpret_cast<const char8_t*>(path)))).wstring());
                            }
                            sqlite3_finalize(sqlite_stmt);

                            if(pDB)
                            {
                                sqlite3_close_v2(pDB);
                                pDB = nullptr;
                            }

                            threadpool.post([](int year, int month, int element, int interval, std::vector<std::wstring> const& db_path){

                                try
                                {

                                BOOST_LOG_TRIVIAL(error)
                                        << "/tongguo/query_condition" << "---"
                                        << "year:" << year << " "
                                        << "month:" << month << " "
                                        << "element:" << element << " "
                                        << "interval:" << interval << " "
                                        << "file count:" << db_path.size() << " "
                                        << "cpu_mask:" << KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask();

                                std::vector<std::wstring> db_path_quotation;
                                std::transform(db_path.begin(), db_path.end(), std::back_inserter(db_path_quotation),[](std::wstring const& element){
                                    return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"") + element + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"\"");
                                });
                                std::wstringstream db_path_quotation_wsstream;
                                std::copy(db_path_quotation.begin(), db_path_quotation.end(), std::ostream_iterator<std::wstring, wchar_t>(db_path_quotation_wsstream, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8" ").c_str()));

    //                            boost::filesystem::path exe_path = boost::process::search_path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"kiwi.fast.batch_d"));
    //                            if(!boost::filesystem::exists(exe_path))
    //                            {
    //                                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"执行文件不存在") << '\n';
    //                                return;
    //                            }

                                boost::process::child child_(
                                            //必须完整路径
                                            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\OtherDocuments\build-kiwi.fast-Desktop_Qt_MinGW_w64_64bit_MSYS2-Debug\bin\kiwi.fast.batch_d.exe)")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --cpu_mask ") + std::to_string(KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu::cpu_mask())
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --name ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"tongguo")
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --year ") + std::to_string(year)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --month ") + std::to_string(month)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --element ") + std::to_string(element)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --interval ") + std::to_string(interval)
                                            + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8" --sqlite_filepath ") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(db_path_quotation_wsstream.str()));

                                child_.wait();
                                int result = child_.exit_code();
                                if(result != 0)
                                {
                                    return;
                                }
                                }
                                catch(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << "[" << *boost::get_error_info<boost::throw_file>(ex) << "][" << *boost::get_error_info<boost::throw_line>(ex) << "]";
                                }
                                catch(std::exception& ex)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程异常：" << ex.what();
                                }
                                catch(...)
                                {
                                    BOOST_LOG_TRIVIAL(error)
                                            << "批处理线程未知异常";
                                }

                            }, year, month, element, interval, db_path);
                        }, std::ref(m_self.m_global_threadpool));

                        ////

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }
                else if(request_target_decode == u8"/tongguo/query_result")
                {
                    std::lock_guard<std::mutex> lg(m_self.m_global_mutex);

                    try
                    {
                        std::string body = boost::beast::buffers_to_string(request.body().data());

                        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongguo> query_result_;
                        query_result_.from_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(body));

                        query_result_.handle([&](int year, int month, int element, int interval, std::u8string const& instrument_type, double lat, double lon
                                             , std::uint64_t latlonrange_count
                                             , std::uint64_t landingdiscrimination_count
                                             , std::uint64_t time_count
                                             , std::uint64_t standardlayerdensitystability_count
                                             , std::uint64_t standardlayertemperaturesalinitystatistics_count
                                             , std::uint64_t measuredlayerdensitystability_count
                                             , std::uint64_t temperaturesalinitygradient_count
                                             , std::uint64_t incremental_count
                                             , std::uint64_t illegalcode_count
                                             , std::uint64_t levelreversalrepetition_count
                                             , std::uint64_t extremum_count
                                             , std::uint64_t total_count){

                            BOOST_LOG_TRIVIAL(error)
                                        << "/tongguo/query_result" << "---"
                                        << "year:" << year << " "
                                        << "month:" << month << " "
                                        << "element:" << element << " "
                                        << "interval:" << interval << " "
                                        << "lat:" << lat << " "
                                        << "lon:" << lon << " "
                                        << "latlonrange_count:" << latlonrange_count << " "
                                        << "landingdiscrimination_count:" << landingdiscrimination_count << " "
                                        << "time_count:" << time_count << " "
                                        << "standardlayerdensitystability_count:" << standardlayerdensitystability_count << " "
                                        << "standardlayertemperaturesalinitystatistics_count:" << standardlayertemperaturesalinitystatistics_count << " "
                                        << "measuredlayerdensitystability_count:" << measuredlayerdensitystability_count << " "
                                        << "temperaturesalinitygradient_count:" << temperaturesalinitygradient_count << " "
                                        << "incremental_count:" << incremental_count << " "
                                        << "illegalcode_count:" << illegalcode_count << " "
                                        << "levelreversalrepetition_count:" << levelreversalrepetition_count << " "
                                        << "extremum_count:" << extremum_count << " "
                                        << "total_count:" << total_count
                                           ;

                            sqlite3* pDB = nullptr;
                            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(u8R"(X:\tongcun\hysw_gyxx.db)").c_str(),
                                                              &pDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL);
                            if(open_result != SQLITE_OK)
                            {
                                return;//打开数据库连接失败
                            }

                            std::u8string sql = u8"INSERT INTO statistics_tongguo(year,month,element,resolution,slat,slon,total_count";
                            sql += u8",latlonrange_count";
                            sql += u8",landingdiscrimination_count";
                            sql += u8",time_count";
                            sql += u8",standardlayerdensitystability_count";
                            sql += u8",standardlayertemperaturesalinitystatistics_count";
                            sql += u8",measuredlayerdensitystability_count";
                            sql += u8",temperaturesalinitygradient_count";
                            sql += u8",incremental_count";
                            sql += u8",illegalcode_count";
                            sql += u8",levelreversalrepetition_count";
                            sql += u8",extremum_count";
                            sql += u8") VALUES(";

                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(year));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(month));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(element));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(interval));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(total_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(latlonrange_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(landingdiscrimination_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(time_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(standardlayerdensitystability_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(standardlayertemperaturesalinitystatistics_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(measuredlayerdensitystability_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(temperaturesalinitygradient_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(incremental_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(illegalcode_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(levelreversalrepetition_count));
                            sql += u8",";
                            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(extremum_count));
                            sql += u8")";

                            sql += u8";";

                            sqlite3_stmt* sqlite_stmt = nullptr;
                            int prepare_result = sqlite3_prepare_v2(
                                        pDB
                                        , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(sql).c_str()
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

                        m_self.m_queue(create_ok_response(request));
                    }
                    catch(...)
                    {
                        return m_self.m_queue(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"操作失败")));
                    }
                }


            }
//            else if(request.method() == boost::beast::http::verb::put)
//            {
//                //更新
//                if(request_target_decode.substr(0, 6) == u8"/dock")
//                {
//                    std::lock_guard<std::mutex> lg(mutex_);

//                    try
//                    {
//                        //...

//                        queue_(create_ok_response(request));
//                    }
//                    catch(...)
//                    {
//                        return queue_(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"操作失败")));
//                    }
//                }
//            }
//            else if(request.method() == boost::beast::http::verb::delete_)
//            {
//                //删除ibox
//                if(request_target_decode.substr(0, 6) == u8"/ibox/")
//                {
//                    std::lock_guard<std::mutex> lg(mutex_);

//                    try
//                    {
//                        //...

//                        queue_(create_ok_response(request));
//                    }
//                    catch(...)
//                    {
//                        return queue_(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"操作失败")));
//                    }
//                }
//            }
//            else if(request.method() == boost::beast::http::verb::get
//                    || request.method() == boost::beast::http::verb::head)
//            {
//                //读取ibox
//                if(request_target_decode.substr(0, 5) == u8"/ibox")
//                {
//                    std::lock_guard<std::mutex> lg(mutex_);

//                    try
//                    {
//                        boost::property_tree::wptree tree;
//                        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.code"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"200001"));
//                        tree.put(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"error.text"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"完成"));

//                        std::wstringstream wsstream;
//                        boost::property_tree::write_xml(wsstream, tree);

//                        boost::beast::http::response<boost::beast::http::basic_string_body<char8_t>> response_{boost::beast::http::status::ok, request.version()};
//                        response_.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
//                        response_.set(boost::beast::http::field::content_type, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"application/xml"));
//                        response_.keep_alive(request.keep_alive());
//                        response_.body() = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(wsstream.str());
//                        response_.prepare_payload();
//                        queue_(std::move(response_));
//                    }
//                    catch(...)
//                    {
//                        return queue_(create_server_error_response(request, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"操作失败")));
//                    }
//                }
//            }

        }

        http_session& m_self;
    };

public:
    http_session(
            boost::asio::ip::tcp::socket&& socket
            , std::string const& server_root
            , std::mutex& global_mutex
            , KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu& global_threadpool
            )
        :m_stream(std::move(socket))
        ,m_server_root(server_root)
        ,m_queue(*this)
        ,m_request_handler(*this)
        ,m_global_mutex(global_mutex)
        ,m_global_threadpool(global_threadpool)
    {
    }

    //开始会话过程
    void run()
    {
        do_read();
    }

    boost::beast::tcp_stream& stream(){return m_stream;}

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
        if(m_queue.on_write())
        {
            //如果队列已经满了，那么当队列减少一个时读下一个请求，可见每个http会话只有一个异步读
            do_read();
        }
    }

private:
    void do_read()
    {
        //为每个请求消息构造一个新的解析器
        m_parser.emplace();

        //这个好像有问题 m_parser->body_limit(std::numeric_limits<std::uint64_t>::max());

        //设置超时时间
        m_stream.expires_after(std::chrono::seconds(20));

        //使用面向解析器接口读取请求信息
        boost::beast::http::async_read(
                    m_stream,
                    m_flat_buffer,
                    *m_parser,
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
        auto request = m_parser->release();
        m_request_handler(std::move(request));

        //如果队列没有满，那么读取另一个请求，这里可以看出每个http会话只有一个异步读，所以即使多线程也不需要同步因为只有一个线程进入on_read，因为每个http会话中只有一个队列，所以同样也只有一个线程进入on_write
        if(!m_queue.is_full())
            do_read();
    }

    void do_close()
    {
        //发送一个TCP shutdown
        boost::beast::error_code ec;
        m_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);

        //这时，连接已标准方式关闭
    }


private:
    boost::beast::tcp_stream m_stream;//拥有套接字所有权
    boost::beast::flat_buffer m_flat_buffer;
    std::string m_server_root;

    queue m_queue;
    request_handler m_request_handler;

    std::mutex& m_global_mutex;
    KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu& m_global_threadpool;

    std::mutex m_session_mutex;

    //使用boost::beast::http::basic_file_body<file>>是为了避免请求和响应中body的大小限制
    //还需要配合 parser_->body_limit(std::numeric_limits<std::uint64_t>::max());
    //boost::optional<boost::beast::http::request_parser<boost::beast::http::basic_file_body<file>>> parser_;

    boost::optional<boost::beast::http::request_parser<boost::beast::http::dynamic_body>> m_parser;
};
