#include <kiwi.fast.client/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>
#include <kiwi.fast.plugin_utility/error_code.h>

#include <kiwi.fast.model/detail/station_count.h>
#include <kiwi.fast.model/detail/cengshen.h>
#include <kiwi.fast.model/detail/tongcun.h>
#include <kiwi.fast.model/detail/chongfu.h>
#include <kiwi.fast.model/detail/tongguo.h>

#include <kiwi.fast.utility/http_request.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <fstream>
#include <cstdint>
#include <string>

//发送站次数统计请求
void statistic_station_count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code* ec = nullptr)
{
    try
    {
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

        send_http_post_request_.set_target(u8"/station_count/query_condition");

        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER station_count> query_condition_;
        for(int year = 2005; year <= 2020; ++year)
        {
            for(int month = 1; month <= 12; ++month)
            {
                {
                    int element = 0;
                    int interval = 2;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 0;
                    int interval = 5;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 1;
                    int interval = 2;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 1;
                    int interval = 5;
                    query_condition_.add(year, month, element, interval);
                }
            }
        }

        boost::property_tree::wptree tree;
        query_condition_.to_property_tree(tree);
        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);
        send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

        if(!send_http_post_request_())
        {
            if(ec != nullptr)
            {
                *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::request_send_error;
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送HTTP请求失败");
            }
        }
    }
    catch(...)
    {
        if(ec != nullptr)
        {
            *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::exception;
        }
        else
        {
            throw;
        }
    }
}

//发送层深统计请求
void statistic_cengshen(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code* ec = nullptr)
{
    try
    {
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

        send_http_post_request_.set_target(u8"/cengshen/query_condition");

        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER cengshen> query_condition_;
        for(int year = 1772; year <= 2020; ++year)
        {
            for(int month = 1; month <= 12; ++month)
            {
                {
                    int element = 0;
                    int interval = 2;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 0;
                    int interval = 5;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 1;
                    int interval = 2;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 1;
                    int interval = 5;
                    query_condition_.add(year, month, element, interval);
                }
            }
        }

        boost::property_tree::wptree tree;
        query_condition_.to_property_tree(tree);
        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);
        send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

        if(!send_http_post_request_())
        {
            if(ec != nullptr)
            {
                *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::request_send_error;
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送HTTP请求失败");
            }
        }
    }
    catch(...)
    {
        if(ec != nullptr)
        {
            *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::exception;
        }
        else
        {
            throw;
        }
    }
}

//发送同存统计请求
void statistic_tongcun(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code* ec = nullptr)
{
    try
    {
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

        send_http_post_request_.set_target(u8"/tongcun/query_condition");

        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongcun> query_condition_;
        for(int year = 1772; year <= 2020; ++year)
        {
            for(int month = 1; month <= 12; ++month)
            {
                {
                    int interval = 2;
                    query_condition_.add(year, month, interval);
                }

                {
                    int interval = 5;
                    query_condition_.add(year, month, interval);
                }
            }
        }

        boost::property_tree::wptree tree;
        query_condition_.to_property_tree(tree);
        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);
        send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

        if(!send_http_post_request_())
        {
            if(ec != nullptr)
            {
                *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::request_send_error;
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送HTTP请求失败");
            }
        }
    }
    catch(...)
    {
        if(ec != nullptr)
        {
            *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::exception;
        }
        else
        {
            throw;
        }
    }
}

//发送重复率统计请求
void statistic_chongfu(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code* ec = nullptr)
{
    try
    {
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

        send_http_post_request_.set_target(u8"/chongfu/query_condition");

        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER chongfu> query_condition_;
        for(int year = 1772; year <= 2020; ++year)
        {
            for(int month = 1; month <= 12; ++month)
            {
                {
                    int element = 0;
                    int interval = 2;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 0;
                    int interval = 5;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 1;
                    int interval = 2;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 1;
                    int interval = 5;
                    query_condition_.add(year, month, element, interval);
                }
            }
        }

        boost::property_tree::wptree tree;
        query_condition_.to_property_tree(tree);
        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);
        send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

        if(!send_http_post_request_())
        {
            if(ec != nullptr)
            {
                *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::request_send_error;
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送HTTP请求失败");
            }
        }
    }
    catch(...)
    {
        if(ec != nullptr)
        {
            *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::exception;
        }
        else
        {
            throw;
        }
    }
}

//发送通过率统计请求
void statistic_tongguo(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code* ec = nullptr)
{
    try
    {
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

        send_http_post_request_.set_target(u8"/tongguo/query_condition");

        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_condition<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongguo> query_condition_;
        for(int year = 1772; year <= 2020; ++year)
        {
            for(int month = 1; month <= 12; ++month)
            {
                //
                {
                    int element = 0;
                    int interval = 2;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 0;
                    int interval = 5;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 1;
                    int interval = 2;
                    query_condition_.add(year, month, element, interval);
                }

                {
                    int element = 1;
                    int interval = 5;
                    query_condition_.add(year, month, element, interval);
                }
            }
        }

        boost::property_tree::wptree tree;
        query_condition_.to_property_tree(tree);
        std::wstringstream wsstream;
        boost::property_tree::write_xml(wsstream, tree);
        send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

        if(!send_http_post_request_())
        {
            if(ec != nullptr)
            {
                *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::request_send_error;
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送HTTP请求失败");
            }
        }
    }
    catch(...)
    {
        if(ec != nullptr)
        {
            *ec = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER error_code_value::exception;
        }
        else
        {
            throw;
        }
    }
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    boost::program_options::options_description options_description_(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"所有选项"), 500);

    /* 选项信息的字符长度是有限制的，因为太长会自动换行，导致截断换行输出乱码 */
    options_description_.add_options()
            //
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"help").c_str()
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"帮助信息").c_str());

    /* 注意设置std::wstring类型选项的默认值时只能使用带两个参数的default_value函数 */

    auto pr = boost::program_options::wcommand_line_parser(argc, argv)
            .options(options_description_)
            /*
             * program_options库使用的程序选项默认是UNIX风格，使用双连字符和单连字符指定选项名，但如果用户喜欢，也可以改为使用Windows风格的斜杠
            */
            .style(boost::program_options::command_line_style::unix_style | boost::program_options::command_line_style::allow_slash_for_short)
            /*
             * 允许未定义选项
            */
            .allow_unregistered()
            .run();

    /*
     * 当使用了allow_unregistered()函数后，command_line_parser遇到未定义但又“像”是选项的命令行参数时，
     * 会把他保存在分析结果parsed_options对象中，而不是抛出异常。这些无法识别的参数可以使用collect_unrecognized()获得
    */
    std::vector<std::wstring> ur_opts = boost::program_options::collect_unrecognized(pr.options, boost::program_options::include_positional);

    boost::program_options::variables_map variables_map_;
    /* 保存解析结果到variables_map_ */
    /* store()函数允许多次调用，实现同时从不同的数据源获得选项信息，比如同时使用命令行和配置文件，已经有值的选项如果再进行store()会自动忽略，所以
     * 对于容器类型的选项，需要指定composing用来合并防止自动忽略
    */
    boost::program_options::store(pr, variables_map_);

    boost::program_options::notify(variables_map_);

    if(variables_map_.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"help")))
    {
        std::cout << options_description_ << '\n';
        return 0;
    }

    statistic_tongcun();

    return 0;
}
