#include <kiwi.fast.server/detail/config.h>

#include <kiwi.fast.server/src/http_session.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <kiwi.fast.utility/threadpool.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <fstream>
#include <cstdint>


void on_accept(
        boost::asio::ip::tcp::acceptor* pacceptor
        , boost::asio::io_context* pio_context
        , std::string const& server_root
        , std::mutex* global_mutex
        , KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu* global_threadpool
        , boost::beast::error_code ec
        , boost::asio::ip::tcp::socket socket)
{
    if(ec)
    {
        BOOST_LOG_TRIVIAL(error) << "accept" << ":" << ec.message();
    }
    else
    {
        //Create the http session and run it
        std::make_shared<http_session>(
                    std::move(socket),
                    server_root,
                    *global_mutex,
                    *global_threadpool
                    )->run();
    }

    //Accept another connection
    pacceptor->async_accept(
                boost::asio::make_strand(*pio_context)
                , boost::beast::bind_front_handler(&on_accept, pacceptor, pio_context, server_root, global_mutex, global_threadpool));
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    //配置日志文件
    //每到文件大小大于10mb或者到午夜，转为另一个文件
    boost::log::add_file_log(
                boost::log::keywords::file_name = "kiwi.fast.server_log_%Y-%m-%d_%H-%M-%S%F.%N.log",
                boost::log::keywords::rotation_size = 10*1024*1024,
                boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0,0,0),
                boost::log::keywords::format = "[%TimeStamp%][%ProcessID%:%ThreadID%]:%Message%"
            );
    boost::log::core::get()->set_filter(
                boost::log::trivial::severity >= boost::log::trivial::info
                );
    boost::log::add_common_attributes();


    //配置命令行参数
    boost::program_options::options_description options_description_(
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"所有选项"), 500);
    options_description_.add_options()
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"server_address").c_str()
             , boost::program_options::wvalue<std::wstring>()->default_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"127.0.0.1"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"127.0.0.1"))
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"服务器地址").c_str())
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"server_port").c_str()
             , boost::program_options::wvalue<unsigned short>()->default_value(8911)
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"服务器端口").c_str())
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"server_root").c_str()
             , boost::program_options::wvalue<std::wstring>()->default_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"."), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"."))
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"服务器根目录").c_str())
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"server_thread").c_str()
             , boost::program_options::wvalue<int>()->default_value(1)
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"服务器线程数").c_str())
            //
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"help").c_str()
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"帮助信息").c_str())
            ;

    auto pr = boost::program_options::wcommand_line_parser(argc, argv)
            .options(options_description_)
            .style(boost::program_options::command_line_style::unix_style | boost::program_options::command_line_style::allow_slash_for_short)
            .allow_unregistered()
            .run();

    std::vector<std::wstring> ur_opts = boost::program_options::collect_unrecognized(pr.options, boost::program_options::include_positional);

    boost::program_options::variables_map variables_map_;
    boost::program_options::store(pr, variables_map_);

    boost::program_options::notify(variables_map_);

    if(variables_map_.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"help")))
    {
        std::cout << options_description_ << '\n';
        return 0;
    }

    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"server_address:") << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(variables_map_["server_address"].as<std::wstring>()) << '\n';
    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"server_port:") << variables_map_["server_port"].as<unsigned short>() << '\n';
    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"server_root:") << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(variables_map_["server_root"].as<std::wstring>()) << '\n';
    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"server_thread:") << variables_map_["server_thread"].as<int>() << '\n';

    ////

    boost::asio::io_context io_context_{variables_map_["server_thread"].as<int>()};

    boost::asio::ip::tcp::acceptor acceptor_(boost::asio::make_strand(io_context_));

    boost::asio::ip::tcp::endpoint endpoint_{
        boost::asio::ip::make_address(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(variables_map_["server_address"].as<std::wstring>()))
        , variables_map_["server_port"].as<unsigned short>()
    };

    boost::beast::error_code ec;
    //Open the acceptor
    acceptor_.open(endpoint_.protocol(), ec);
    if(ec)
    {
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER runtime_error, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(ec.message()));
    }

    //Allow address reuse
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if(ec)
    {
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER runtime_error, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(ec.message()));
    }

    //Bind to the server address
    acceptor_.bind(endpoint_, ec);
    if(ec)
    {
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER runtime_error, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(ec.message()));
    }

    //Start listening for connections
    acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
    if(ec)
    {
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER runtime_error, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(ec.message()));
    }

    std::string server_root = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(variables_map_["server_root"].as<std::wstring>());

    std::mutex mutex_;
    KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER threadpool_per_cpu threadpool_;

    //The new connection gets its own strand
    acceptor_.async_accept(
                boost::asio::make_strand(io_context_)
                , boost::beast::bind_front_handler(&on_accept, &acceptor_, &io_context_, server_root, &mutex_, &threadpool_));

    //捕获SIGINT和SIGTERM信号，执行一个干净的退出
    boost::asio::signal_set signals(io_context_, SIGINT, SIGTERM);
    signals.async_wait(
                [&](boost::beast::error_code const&, int)
    {
        io_context_.stop();
    });

    std::vector<std::thread> v;
    v.reserve(variables_map_["server_thread"].as<int>() - 1);
    for(auto i = variables_map_["server_thread"].as<int>() - 1; i > 0; --i)
    {
        v.emplace_back(
                    [&io_context_]{
            io_context_.run();
        });
    }
    io_context_.run();

    for(auto& t:v)
    {
        t.join();
    }

    return 0;
}
