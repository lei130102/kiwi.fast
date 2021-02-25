#include <kiwi.fast.batch/detail/config.h>

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <sqlite3.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <kiwi.fast.model/detail/station_count.h>

#include <kiwi.fast.utility/http_request.h>

#include <fstream>
#include <cstdint>
#include <vector>
#include <string>

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    //配置日志文件
    //每到文件大小大于10mb或者到午夜，转为另一个文件
    boost::log::add_file_log(
                boost::log::keywords::file_name = "kiwi.fast.batch_log_%Y-%m-%d_%H-%M-%S.%N.log",
                boost::log::keywords::rotation_size = 10*1024*1024,
                boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0,0,0),
                boost::log::keywords::format = "[%TimeStamp%][%ProcessID%:%ThreadID%]:%Message%"
            );
    boost::log::core::get()->set_filter(
                boost::log::trivial::severity >= boost::log::trivial::info
                );
    boost::log::add_common_attributes();

    //配置命令行参数
    boost::program_options::options_description options_description_(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"所有选项"), 500);
    /* 选项信息的字符长度是有限制的，因为太长会自动换行，导致截断换行输出乱码 */
    options_description_.add_options()
            //cpu_mask
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"cpu_mask").c_str()
             , boost::program_options::wvalue<int>()
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"cpu mask").c_str())
            //year
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"year").c_str()
             , boost::program_options::wvalue<int>()
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"年份").c_str())
            //month
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"month").c_str()
             , boost::program_options::wvalue<int>()
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"月份").c_str())
            //element
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"element").c_str()
             , boost::program_options::wvalue<int>()
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"元素类型：0-温度 1-盐度").c_str())
            //interval
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"interval").c_str()
             , boost::program_options::wvalue<int>()
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"分辨率").c_str())
            //sqlite_filepath
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"sqlite_filepath").c_str()
             , boost::program_options::wvalue<std::vector<std::wstring>>()->zero_tokens()->multitoken()->composing()
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"sqlite数据库文件路径").c_str())
            //
            (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"help").c_str()
             , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"帮助信息").c_str());

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
    /* 把值更新到用户指定的变量 */
    boost::program_options::notify(variables_map_);

    /* variables_map的键类型是string，他存放了选项描述器option_description定义的选项名。
     * 在使用variables_map时我们只能使用选项的长名，使用短名索引会发生运行时异常
     *
     * variables_map的值类型是any对象的一个包装类variable_value，他的成员函数as<T>()包
     * 装了any_cast<T>()，取出any内存放的选项值，因此as<T>()函数的模板类型必须与存储的类型相符，否则会抛出bad_any_cast异常
    */

    if(variables_map_.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"help")))
    {
        std::cout << options_description_ << '\n';
        return 0;
    }

    bool is_debug = false;

    boost::optional<int> cpu_mask;
    boost::optional<int> year;
    boost::optional<int> month;
    boost::optional<int> element;
    boost::optional<int> interval;
    std::vector<std::wstring> sqlite_filepath;
    if(is_debug)
    {
        //测试
        cpu_mask = 0x00000002;
        year = 1999;
        month = 6;
        element = 0;
        interval = 2;

        sqlite_filepath.push_back(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8R"(F:\DQ1028Data\海洋水文资料\海洋温盐\国际业务化资料\信息中心\历史\Basic\S1000-Y1999-TS.dat.db)"));
        sqlite_filepath.push_back(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8R"(F:\DQ1028Data\海洋水文资料\海洋温盐\国际业务化资料\信息中心\历史\Basic\S1004-Y1999-TS.dat.db)"));
        sqlite_filepath.push_back(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8R"(F:\DQ1028Data\海洋水文资料\海洋温盐\国际业务化资料\信息中心\历史\Basic\S1005-Y1999-TS.dat.db)"));
    }
    else
    {
        //cpu_mask
        if(!variables_map_.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"cpu_mask")))
        {
            BOOST_LOG_TRIVIAL(error) << "没有提供cpu_mask选项的值";

            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供cpu_mask选项的值");
        }
        cpu_mask = variables_map_["cpu_mask"].as<int>();
        BOOST_LOG_TRIVIAL(error) << "year:" << year;
        //year
        if(!variables_map_.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"year")))
        {
            BOOST_LOG_TRIVIAL(error) << "没有提供year选项的值";

            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供year选项的值");
        }
        year = variables_map_["year"].as<int>();
        BOOST_LOG_TRIVIAL(error) << "year:" << year;

        //month
        if(!variables_map_.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"month")))
        {
            BOOST_LOG_TRIVIAL(error) << "没有提供month选项的值";

            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供month选项的值");
        }
        month = variables_map_["month"].as<int>();
        BOOST_LOG_TRIVIAL(error) << "month:" << month;

        //element
        if(!variables_map_.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"element")))
        {
            BOOST_LOG_TRIVIAL(error) << "没有提供element选项的值";

            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供element选项的值");
        }
        element = variables_map_["element"].as<int>();
        BOOST_LOG_TRIVIAL(error) << "element:" << element;

        //interval
        if(!variables_map_.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"interval")))
        {
            BOOST_LOG_TRIVIAL(error) << "没有提供interval选项的值";

            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供interval选项的值");
        }
        interval = variables_map_["interval"].as<int>();
        BOOST_LOG_TRIVIAL(error) << "interval:" << interval;

        //sqlite_filepath
        if(!variables_map_.count(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(u8"sqlite_filepath")))
        {
            BOOST_LOG_TRIVIAL(error) << "没有提供sqlite_filepath选项的值";

            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供sqlite_filepath选项的值");
        }
        sqlite_filepath = variables_map_["sqlite_filepath"].as<std::vector<std::wstring>>();
        std::wstringstream wsstream;
        std::copy(sqlite_filepath.begin(), sqlite_filepath.end(), std::ostream_iterator<std::wstring, wchar_t>(wsstream, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8" ").c_str()));
        BOOST_LOG_TRIVIAL(error) << "sqlite_filepath:" << reinterpret_cast<const char*>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(wsstream.str()).c_str());
    }

    SetProcessAffinityMask(GetCurrentProcess(), cpu_mask.get());

    BOOST_LOG_TRIVIAL(error) << "sqlite file count:" << sqlite_filepath.size();

    //////
    KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER station_count> query_result_;
    for(auto const& filepath : sqlite_filepath)
    {
        BOOST_LOG_TRIVIAL(error) << "sqlite file path:" << reinterpret_cast<const char*>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(filepath).c_str());

        boost::filesystem::path sqlite_filepath_path(filepath);

        if(!boost::filesystem::exists(sqlite_filepath_path))
        {
            continue;
        }

        sqlite3* pDB = nullptr;
        int open_result = sqlite3_open_v2(reinterpret_cast<const char*>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(sqlite_filepath_path.native()).c_str())
                , &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
        if(open_result != SQLITE_OK)
        {
            BOOST_LOG_TRIVIAL(error) << "打开数据库连接失败";

            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"打开数据库连接失败：") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(open_result)));
        }

        std::u8string sql = u8"SELECT subtable.Lat, subtable.Lon, count(*) StationCount FROM ";

        sql += u8"(";
        sql += u8"SELECT ";

        sql += u8" CASE";
        for(int lat = 90; lat >= -90; lat -= interval.get())
        {
            for(int lon = -180; lon <= 180; lon += interval.get())
            {
                sql += u8" WHEN s.Lat BETWEEN ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lat - interval.get()));
                sql += u8" AND ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lat));
                sql += u8" AND s.Lon BETWEEN ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lon));
                sql += u8" AND ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lon + interval.get()));
                sql += u8" THEN ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lat));
            }
        }
        sql += u8" END Lat";
        sql += u8",";
        sql += u8" CASE";
        for(int lat = 90; lat >= -90; lat -= interval.get())
        {
            for(int lon = -180; lon <= 180; lon += interval.get())
            {
                sql += u8" WHEN s.Lat BETWEEN ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lat - interval.get()));
                sql += u8" AND ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lat));
                sql += u8" AND s.Lon BETWEEN ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lon));
                sql += u8" AND ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lon + interval.get()));
                sql += u8" THEN ";
                sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(std::to_wstring(lon));
            }
        }
        sql += u8" END Lon";
        sql += u8" FROM (Station s INNER JOIN Record r ON r.StationID=s.ID) INNER JOIN Cruise c ON s.Cruise_RowID=c.ID WHERE 1=1 ";
        sql += u8" AND c.SurveyProject='3003' ";

        boost::gregorian::date start_date(year.get(), month.get(), 1);
        boost::gregorian::date end_date(start_date.end_of_month());
        sql += u8" AND (s.ObsDate BETWEEN '";
        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(boost::gregorian::to_sql_string(start_date));
        sql += u8"' AND '";
        sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(boost::gregorian::to_sql_string(end_date));
        sql += u8"')";

        if(element == 0)
        {
            sql += u8" AND r.WaterTemp<>999999999";
        }
        else if(element == 1)
        {
            sql += u8" AND r.Salinity<>999999999";
        }

        sql += u8" AND s.LatLonQuaTag=0";

        sql += u8") subtable";

        sql += u8" GROUP BY subtable.Lat, subtable.Lon;";

        sqlite3_stmt* sqlite_stmt = nullptr;
        int prepare_result = sqlite3_prepare_v2(
                    pDB
                    , reinterpret_cast<const char*>(sql.c_str())
                    , -1
                    , &sqlite_stmt
                    , NULL);
        if(prepare_result != SQLITE_OK)
        {
            BOOST_LOG_TRIVIAL(error) << "sql error";

            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"查询语句错误");
        }

        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER station_count> tmp_query_result;
        while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
        {
            tmp_query_result.add(year.get(), month.get(), element.get(), interval.get(), sqlite3_column_double(sqlite_stmt, 0)
                                 , sqlite3_column_double(sqlite_stmt, 1)
                                 , sqlite3_column_int64(sqlite_stmt, 2));
        }
        sqlite3_finalize(sqlite_stmt);


        if(pDB)
        {
            sqlite3_close_v2(pDB);
            pDB = nullptr;
        }

        query_result_.merge(tmp_query_result);
    }

    BOOST_LOG_TRIVIAL(error) << "sql finished";

    //通过服务器上传数据query_result_

    KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

    send_http_post_request_.set_target(u8"/station_count/query_result");

    boost::property_tree::wptree tree;
    query_result_.to_property_tree(tree);
    std::wstringstream wsstream;
    boost::property_tree::write_xml(wsstream, tree);
    send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8(wsstream.str()));

    if(!send_http_post_request_())
    {
        BOOST_LOG_TRIVIAL(error) << "update error";

        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送结果请求失败");
    }

    BOOST_LOG_TRIVIAL(error) << "update finished";

    return 0;
}
