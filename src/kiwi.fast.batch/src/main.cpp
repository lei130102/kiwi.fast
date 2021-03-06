#include <kiwi.fast.batch/detail/config.h>

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <sqlite3.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <kiwi.fast.utility/command_line.h>

#include <kiwi.fast.model/detail/station_count.h>
#include <kiwi.fast.model/detail/cengshen.h>
#include <kiwi.fast.model/detail/tongcun.h>
#include <kiwi.fast.model/detail/chongfu.h>
#include <kiwi.fast.model/detail/tongguo.h>

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
                boost::log::keywords::file_name = "kiwi.fast.batch_log_%Y-%m-%d_%H-%M-%S%F.%N.log",
                boost::log::keywords::rotation_size = 10*1024*1024,
                boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0,0,0),
                boost::log::keywords::format = "[%TimeStamp%][%ProcessID%:%ThreadID%]:%Message%"
            );
    boost::log::core::get()->set_filter(
                boost::log::trivial::severity >= boost::log::trivial::info
                );
    boost::log::add_common_attributes();

    KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER command_line* command_line_ = KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER command_line::instance();
    command_line_->add_option<char8_t, int>(u8"cpu_mask", u8"CPU核掩码");
    command_line_->add_option<char8_t, std::wstring>(u8"name", u8"批处理任务名称");
    command_line_->add_option<char8_t, int>(u8"year", u8"年份");
    command_line_->add_option<char8_t, int>(u8"month", u8"月份");
    command_line_->add_option<char8_t, int>(u8"element", u8"要素类型：0-温度 1-盐度");
    command_line_->add_option<char8_t, int>(u8"interval", u8"分辨率：2-2°*2° 5-5°*5°");
    command_line_->add_option<char8_t, std::deque<std::wstring>>(u8"sqlite_filepath", u8"sqlite数据库文件路径");
    command_line_->add_option<char8_t>(u8"help", u8"帮助信息");
    command_line_->run(argc, argv, envp);

    if(command_line_->variable_count(std::u8string(u8"help")))
    {
        command_line_->output_options_description();
        return 0;
    }

    //cpu_mask
    std::optional<int> cpu_mask = command_line_->variable<char8_t, int>(u8"cpu_mask");
    if(!cpu_mask)
    {
        BOOST_LOG_TRIVIAL(error) << "没有提供cpu_mask选项的值";
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供cpu_mask选项的值");
        return 0;
    }

    //name
    std::optional<std::wstring> name = command_line_->variable<char8_t, std::wstring>(u8"name");
    if(!name)
    {
        BOOST_LOG_TRIVIAL(error) << "没有提供name选项的值";
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供name选项的值");
        return 0;
    }

    std::optional<int> year = command_line_->variable<char8_t, int>(u8"year");
    if(!year)
    {
        BOOST_LOG_TRIVIAL(error) << "没有提供year选项的值";
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供year选项的值");
        return 0;
    }

    std::optional<int> month = command_line_->variable<char8_t, int>(u8"month");
    if(!month)
    {
        BOOST_LOG_TRIVIAL(error) << "没有提供month选项的值";
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供month选项的值");
        return 0;
    }
    std::optional<int> element = command_line_->variable<char8_t, int>(u8"element");
    if(!element)
    {
        BOOST_LOG_TRIVIAL(error) << "没有提供element选项的值";
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供element选项的值");
        return 0;
    }
    std::optional<int> interval = command_line_->variable<char8_t, int>(u8"interval");
    if(!interval)
    {
        BOOST_LOG_TRIVIAL(error) << "没有提供interval选项的值";
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供interval选项的值");
        return 0;
    }
    std::optional<std::deque<std::wstring>> sqlite_filepath = command_line_->variable<char8_t, std::deque<std::wstring>>(u8"sqlite_filepath");
    if(!sqlite_filepath)
    {
        BOOST_LOG_TRIVIAL(error) << "没有提供sqlite_filepath选项的值";
        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"没有提供sqlite_filepath选项的值");
        return 0;
    }

    SetProcessAffinityMask(GetCurrentProcess(), *cpu_mask);

    ///////////////////////////////////////////////////////////

    if(name == KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"station_count"))
    {
        BOOST_LOG_TRIVIAL(error) << "sqlite file count:" << *sqlite_filepath;

        //////
        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER station_count> query_result_;
        for(auto const& filepath : *sqlite_filepath)
        {
            BOOST_LOG_TRIVIAL(error) << "sqlite file path:" << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(filepath).c_str();

            boost::filesystem::path sqlite_filepath_path(filepath);

            if(!boost::filesystem::exists(sqlite_filepath_path))
            {
                continue;
            }

            sqlite3* pDB = nullptr;
            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(sqlite_filepath_path.native()).c_str()
                    , &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
            if(open_result != SQLITE_OK)
            {
                BOOST_LOG_TRIVIAL(error) << "打开数据库连接失败";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"打开数据库连接失败：") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(open_result)));
            }

            std::u8string sql = u8"SELECT subtable.Lat, subtable.Lon, count(*) StationCount FROM ";

            sql += u8"(";
            sql += u8"SELECT ";

            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                }
            }
            sql += u8" END Lat";
            sql += u8",";
            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                }
            }
            sql += u8" END Lon";
            sql += u8" FROM (Station s INNER JOIN Record r ON r.StationID=s.ID) INNER JOIN Cruise c ON s.Cruise_RowID=c.ID WHERE 1=1 ";
            sql += u8" AND c.SurveyProject='3003' ";

            boost::gregorian::date start_date(*year, *month, 1);
            boost::gregorian::date end_date(start_date.end_of_month());
            sql += u8" AND (s.ObsDate BETWEEN '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
            sql += u8"' AND '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
            sql += u8"')";

            if(element == 0)
            {
                sql += u8" AND r.WaterTemp<>9999";
            }
            else if(element == 1)
            {
                sql += u8" AND r.Salinity<>9999";
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
                tmp_query_result.add(*year, *month, *element, *interval, sqlite3_column_double(sqlite_stmt, 0)
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

        if(query_result_.size() > 0)
        {
            //通过服务器上传数据query_result_

            KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

            send_http_post_request_.set_target(u8"/station_count/query_result");

            boost::property_tree::wptree tree;
            query_result_.to_property_tree(tree);
            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);
            send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

            if(!send_http_post_request_())
            {
                BOOST_LOG_TRIVIAL(error) << "update error";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送结果请求失败");
            }
        }

        BOOST_LOG_TRIVIAL(error) << "update finished";
    }
    else if(name == KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"cengshen"))
    {
        BOOST_LOG_TRIVIAL(error) << "sqlite file count:" << (*sqlite_filepath).size();

        //////
        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER cengshen> query_result_;
        for(auto const& filepath : *sqlite_filepath)
        {
            BOOST_LOG_TRIVIAL(error) << "sqlite file path:" << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(filepath).c_str();

            boost::filesystem::path sqlite_filepath_path(filepath);

            if(!boost::filesystem::exists(sqlite_filepath_path))
            {
                continue;
            }

            sqlite3* pDB = nullptr;
            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(sqlite_filepath_path.native()).c_str()
                    , &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
            if(open_result != SQLITE_OK)
            {
                BOOST_LOG_TRIVIAL(error) << "打开数据库连接失败";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"打开数据库连接失败：") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(open_result)));
            }

            std::u8string sql = u8"SELECT subtable.Lat, subtable.Lon, count(*) AllCount,";

            sql += u8"count(subtable.cengshen0) cengshen0,";
            sql += u8"count(subtable.cengshen5) cengshen5,";
            sql += u8"count(subtable.cengshen10) cengshen10,";
            sql += u8"count(subtable.cengshen15) cengshen15,";
            sql += u8"count(subtable.cengshen20) cengshen20,";
            sql += u8"count(subtable.cengshen25) cengshen25,";
            sql += u8"count(subtable.cengshen30) cengshen30,";
            sql += u8"count(subtable.cengshen35) cengshen35,";
            sql += u8"count(subtable.cengshen40) cengshen40,";
            sql += u8"count(subtable.cengshen45) cengshen45,";
            sql += u8"count(subtable.cengshen50) cengshen50,";
            sql += u8"count(subtable.cengshen55) cengshen55,";
            sql += u8"count(subtable.cengshen60) cengshen60,";
            sql += u8"count(subtable.cengshen65) cengshen65,";
            sql += u8"count(subtable.cengshen70) cengshen70,";
            sql += u8"count(subtable.cengshen75) cengshen75,";
            sql += u8"count(subtable.cengshen80) cengshen80,";
            sql += u8"count(subtable.cengshen85) cengshen85,";
            sql += u8"count(subtable.cengshen90) cengshen90,";
            sql += u8"count(subtable.cengshen95) cengshen95,";
            sql += u8"count(subtable.cengshen100) cengshen100,";
            sql += u8"count(subtable.cengshen125) cengshen125,";
            sql += u8"count(subtable.cengshen150) cengshen150,";
            sql += u8"count(subtable.cengshen175) cengshen175,";
            sql += u8"count(subtable.cengshen200) cengshen200,";
            sql += u8"count(subtable.cengshen225) cengshen225,";
            sql += u8"count(subtable.cengshen250) cengshen250,";
            sql += u8"count(subtable.cengshen275) cengshen275,";
            sql += u8"count(subtable.cengshen300) cengshen300,";
            sql += u8"count(subtable.cengshen325) cengshen325,";
            sql += u8"count(subtable.cengshen350) cengshen350,";
            sql += u8"count(subtable.cengshen375) cengshen375,";
            sql += u8"count(subtable.cengshen400) cengshen400,";
            sql += u8"count(subtable.cengshen425) cengshen425,";
            sql += u8"count(subtable.cengshen450) cengshen450,";
            sql += u8"count(subtable.cengshen475) cengshen475,";
            sql += u8"count(subtable.cengshen500) cengshen500,";
            sql += u8"count(subtable.cengshen550) cengshen550,";
            sql += u8"count(subtable.cengshen600) cengshen600,";
            sql += u8"count(subtable.cengshen650) cengshen650,";
            sql += u8"count(subtable.cengshen700) cengshen700,";
            sql += u8"count(subtable.cengshen750) cengshen750,";
            sql += u8"count(subtable.cengshen800) cengshen800,";
            sql += u8"count(subtable.cengshen850) cengshen850,";
            sql += u8"count(subtable.cengshen900) cengshen900,";
            sql += u8"count(subtable.cengshen950) cengshen950,";
            sql += u8"count(subtable.cengshen1000) cengshen1000,";
            sql += u8"count(subtable.cengshen1050) cengshen1050,";
            sql += u8"count(subtable.cengshen1100) cengshen1100,";
            sql += u8"count(subtable.cengshen1150) cengshen1150,";
            sql += u8"count(subtable.cengshen1200) cengshen1200,";
            sql += u8"count(subtable.cengshen1250) cengshen1250,";
            sql += u8"count(subtable.cengshen1300) cengshen1300,";
            sql += u8"count(subtable.cengshen1350) cengshen1350,";
            sql += u8"count(subtable.cengshen1400) cengshen1400,";
            sql += u8"count(subtable.cengshen1450) cengshen1450,";
            sql += u8"count(subtable.cengshen1500) cengshen1500,";
            sql += u8"count(subtable.cengshen1550) cengshen1550,";
            sql += u8"count(subtable.cengshen1600) cengshen1600,";
            sql += u8"count(subtable.cengshen1650) cengshen1650,";
            sql += u8"count(subtable.cengshen1700) cengshen1700,";
            sql += u8"count(subtable.cengshen1750) cengshen1750,";
            sql += u8"count(subtable.cengshen1800) cengshen1800,";
            sql += u8"count(subtable.cengshen1850) cengshen1850,";
            sql += u8"count(subtable.cengshen1900) cengshen1900,";
            sql += u8"count(subtable.cengshen1950) cengshen1950,";
            sql += u8"count(subtable.cengshen2000) cengshen2000,";
            sql += u8"count(subtable.cengshen2100) cengshen2100,";
            sql += u8"count(subtable.cengshen2200) cengshen2200,";
            sql += u8"count(subtable.cengshen2300) cengshen2300,";
            sql += u8"count(subtable.cengshen2400) cengshen2400,";
            sql += u8"count(subtable.cengshen2500) cengshen2500,";
            sql += u8"count(subtable.cengshen2600) cengshen2600,";
            sql += u8"count(subtable.cengshen2700) cengshen2700,";
            sql += u8"count(subtable.cengshen2800) cengshen2800,";
            sql += u8"count(subtable.cengshen2900) cengshen2900,";
            sql += u8"count(subtable.cengshen3000) cengshen3000,";
            sql += u8"count(subtable.cengshen3100) cengshen3100,";
            sql += u8"count(subtable.cengshen3200) cengshen3200,";
            sql += u8"count(subtable.cengshen3300) cengshen3300,";
            sql += u8"count(subtable.cengshen3400) cengshen3400,";
            sql += u8"count(subtable.cengshen3500) cengshen3500,";
            sql += u8"count(subtable.cengshen3600) cengshen3600,";
            sql += u8"count(subtable.cengshen3700) cengshen3700,";
            sql += u8"count(subtable.cengshen3800) cengshen3800,";
            sql += u8"count(subtable.cengshen3900) cengshen3900,";
            sql += u8"count(subtable.cengshen4000) cengshen4000,";
            sql += u8"count(subtable.cengshen4100) cengshen4100,";
            sql += u8"count(subtable.cengshen4200) cengshen4200,";
            sql += u8"count(subtable.cengshen4300) cengshen4300,";
            sql += u8"count(subtable.cengshen4400) cengshen4400,";
            sql += u8"count(subtable.cengshen4500) cengshen4500,";
            sql += u8"count(subtable.cengshen4600) cengshen4600,";
            sql += u8"count(subtable.cengshen4700) cengshen4700,";
            sql += u8"count(subtable.cengshen4800) cengshen4800,";
            sql += u8"count(subtable.cengshen4900) cengshen4900,";
            sql += u8"count(subtable.cengshen5000) cengshen5000,";
            sql += u8"count(subtable.cengshen5100) cengshen5100,";
            sql += u8"count(subtable.cengshen5200) cengshen5200,";
            sql += u8"count(subtable.cengshen5300) cengshen5300,";
            sql += u8"count(subtable.cengshen5400) cengshen5400,";
            sql += u8"count(subtable.cengshen5500) cengshen5500,";
            sql += u8"count(subtable.cengshen5600) cengshen5600,";
            sql += u8"count(subtable.cengshen5700) cengshen5700,";
            sql += u8"count(subtable.cengshen5800) cengshen5800,";
            sql += u8"count(subtable.cengshen5900) cengshen5900,";
            sql += u8"count(subtable.cengshen6000) cengshen6000,";
            sql += u8"count(subtable.cengshen6100) cengshen6100,";
            sql += u8"count(subtable.cengshen6200) cengshen6200,";
            sql += u8"count(subtable.cengshen6300) cengshen6300,";
            sql += u8"count(subtable.cengshen6400) cengshen6400,";
            sql += u8"count(subtable.cengshen6500) cengshen6500,";
            sql += u8"count(subtable.cengshen6600) cengshen6600,";
            sql += u8"count(subtable.cengshen6700) cengshen6700,";
            sql += u8"count(subtable.cengshen6800) cengshen6800,";
            sql += u8"count(subtable.cengshen6900) cengshen6900,";
            sql += u8"count(subtable.cengshen7000) cengshen7000,";
            sql += u8"count(subtable.cengshen7100) cengshen7100,";
            sql += u8"count(subtable.cengshen7200) cengshen7200,";
            sql += u8"count(subtable.cengshen7300) cengshen7300,";
            sql += u8"count(subtable.cengshen7400) cengshen7400,";
            sql += u8"count(subtable.cengshen7500) cengshen7500,";
            sql += u8"count(subtable.cengshen7600) cengshen7600,";
            sql += u8"count(subtable.cengshen7700) cengshen7700,";
            sql += u8"count(subtable.cengshen7800) cengshen7800,";
            sql += u8"count(subtable.cengshen7900) cengshen7900,";
            sql += u8"count(subtable.cengshen8000) cengshen8000,";
            sql += u8"count(subtable.cengshen8100) cengshen8100,";
            sql += u8"count(subtable.cengshen8200) cengshen8200,";
            sql += u8"count(subtable.cengshen8300) cengshen8300,";
            sql += u8"count(subtable.cengshen8400) cengshen8400,";
            sql += u8"count(subtable.cengshen8500) cengshen8500,";
            sql += u8"count(subtable.cengshen8600) cengshen8600,";
            sql += u8"count(subtable.cengshen8700) cengshen8700,";
            sql += u8"count(subtable.cengshen8800) cengshen8800,";
            sql += u8"count(subtable.cengshen8900) cengshen8900,";
            sql += u8"count(subtable.cengshen9000) cengshen9000";

            sql += u8" FROM ";

            sql += u8"(";
            sql += u8"SELECT ";

            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                }
            }
            sql += u8" END Lat";
            sql += u8",";
            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                }
            }
            sql += u8" END Lon";
            sql += u8", CASE WHEN r.Depth<0 OR r.Depth>=5 THEN NULL ELSE 1 END cengshen0";
            sql += u8", CASE WHEN r.Depth<5 OR r.Depth>=10 THEN NULL ELSE 1 END cengshen5";
            sql += u8", CASE WHEN r.Depth<10 OR r.Depth>=15 THEN NULL ELSE 1 END cengshen10";
            sql += u8", CASE WHEN r.Depth<15 OR r.Depth>=20 THEN NULL ELSE 1 END cengshen15";
            sql += u8", CASE WHEN r.Depth<20 OR r.Depth>=25 THEN NULL ELSE 1 END cengshen20";
            sql += u8", CASE WHEN r.Depth<25 OR r.Depth>=30 THEN NULL ELSE 1 END cengshen25";
            sql += u8", CASE WHEN r.Depth<30 OR r.Depth>=35 THEN NULL ELSE 1 END cengshen30";
            sql += u8", CASE WHEN r.Depth<35 OR r.Depth>=40 THEN NULL ELSE 1 END cengshen35";
            sql += u8", CASE WHEN r.Depth<40 OR r.Depth>=45 THEN NULL ELSE 1 END cengshen40";
            sql += u8", CASE WHEN r.Depth<45 OR r.Depth>=50 THEN NULL ELSE 1 END cengshen45";
            sql += u8", CASE WHEN r.Depth<50 OR r.Depth>=55 THEN NULL ELSE 1 END cengshen50";
            sql += u8", CASE WHEN r.Depth<55 OR r.Depth>=60 THEN NULL ELSE 1 END cengshen55";
            sql += u8", CASE WHEN r.Depth<60 OR r.Depth>=65 THEN NULL ELSE 1 END cengshen60";
            sql += u8", CASE WHEN r.Depth<65 OR r.Depth>=70 THEN NULL ELSE 1 END cengshen65";
            sql += u8", CASE WHEN r.Depth<70 OR r.Depth>=75 THEN NULL ELSE 1 END cengshen70";
            sql += u8", CASE WHEN r.Depth<75 OR r.Depth>=80 THEN NULL ELSE 1 END cengshen75";
            sql += u8", CASE WHEN r.Depth<80 OR r.Depth>=85 THEN NULL ELSE 1 END cengshen80";
            sql += u8", CASE WHEN r.Depth<85 OR r.Depth>=90 THEN NULL ELSE 1 END cengshen85";
            sql += u8", CASE WHEN r.Depth<90 OR r.Depth>=95 THEN NULL ELSE 1 END cengshen90";
            sql += u8", CASE WHEN r.Depth<95 OR r.Depth>=100 THEN NULL ELSE 1 END cengshen95";
            sql += u8", CASE WHEN r.Depth<100 OR r.Depth>=125 THEN NULL ELSE 1 END cengshen100";
            sql += u8", CASE WHEN r.Depth<125 OR r.Depth>=150 THEN NULL ELSE 1 END cengshen125";
            sql += u8", CASE WHEN r.Depth<150 OR r.Depth>=175 THEN NULL ELSE 1 END cengshen150";
            sql += u8", CASE WHEN r.Depth<175 OR r.Depth>=200 THEN NULL ELSE 1 END cengshen175";
            sql += u8", CASE WHEN r.Depth<200 OR r.Depth>=225 THEN NULL ELSE 1 END cengshen200";
            sql += u8", CASE WHEN r.Depth<225 OR r.Depth>=250 THEN NULL ELSE 1 END cengshen225";
            sql += u8", CASE WHEN r.Depth<250 OR r.Depth>=275 THEN NULL ELSE 1 END cengshen250";
            sql += u8", CASE WHEN r.Depth<275 OR r.Depth>=300 THEN NULL ELSE 1 END cengshen275";
            sql += u8", CASE WHEN r.Depth<300 OR r.Depth>=325 THEN NULL ELSE 1 END cengshen300";
            sql += u8", CASE WHEN r.Depth<325 OR r.Depth>=350 THEN NULL ELSE 1 END cengshen325";
            sql += u8", CASE WHEN r.Depth<350 OR r.Depth>=375 THEN NULL ELSE 1 END cengshen350";
            sql += u8", CASE WHEN r.Depth<375 OR r.Depth>=400 THEN NULL ELSE 1 END cengshen375";
            sql += u8", CASE WHEN r.Depth<400 OR r.Depth>=425 THEN NULL ELSE 1 END cengshen400";
            sql += u8", CASE WHEN r.Depth<425 OR r.Depth>=450 THEN NULL ELSE 1 END cengshen425";
            sql += u8", CASE WHEN r.Depth<450 OR r.Depth>=475 THEN NULL ELSE 1 END cengshen450";
            sql += u8", CASE WHEN r.Depth<475 OR r.Depth>=500 THEN NULL ELSE 1 END cengshen475";
            sql += u8", CASE WHEN r.Depth<500 OR r.Depth>=550 THEN NULL ELSE 1 END cengshen500";
            sql += u8", CASE WHEN r.Depth<550 OR r.Depth>=600 THEN NULL ELSE 1 END cengshen550";
            sql += u8", CASE WHEN r.Depth<600 OR r.Depth>=650 THEN NULL ELSE 1 END cengshen600";
            sql += u8", CASE WHEN r.Depth<650 OR r.Depth>=700 THEN NULL ELSE 1 END cengshen650";
            sql += u8", CASE WHEN r.Depth<700 OR r.Depth>=750 THEN NULL ELSE 1 END cengshen700";
            sql += u8", CASE WHEN r.Depth<750 OR r.Depth>=800 THEN NULL ELSE 1 END cengshen750";
            sql += u8", CASE WHEN r.Depth<800 OR r.Depth>=850 THEN NULL ELSE 1 END cengshen800";
            sql += u8", CASE WHEN r.Depth<850 OR r.Depth>=900 THEN NULL ELSE 1 END cengshen850";
            sql += u8", CASE WHEN r.Depth<900 OR r.Depth>=950 THEN NULL ELSE 1 END cengshen900";
            sql += u8", CASE WHEN r.Depth<950 OR r.Depth>=1000 THEN NULL ELSE 1 END cengshen950";
            sql += u8", CASE WHEN r.Depth<1000 OR r.Depth>=1050 THEN NULL ELSE 1 END cengshen1000";
            sql += u8", CASE WHEN r.Depth<1050 OR r.Depth>=1100 THEN NULL ELSE 1 END cengshen1050";
            sql += u8", CASE WHEN r.Depth<1100 OR r.Depth>=1150 THEN NULL ELSE 1 END cengshen1100";
            sql += u8", CASE WHEN r.Depth<1150 OR r.Depth>=1200 THEN NULL ELSE 1 END cengshen1150";
            sql += u8", CASE WHEN r.Depth<1200 OR r.Depth>=1250 THEN NULL ELSE 1 END cengshen1200";
            sql += u8", CASE WHEN r.Depth<1250 OR r.Depth>=1300 THEN NULL ELSE 1 END cengshen1250";
            sql += u8", CASE WHEN r.Depth<1300 OR r.Depth>=1350 THEN NULL ELSE 1 END cengshen1300";
            sql += u8", CASE WHEN r.Depth<1350 OR r.Depth>=1400 THEN NULL ELSE 1 END cengshen1350";
            sql += u8", CASE WHEN r.Depth<1400 OR r.Depth>=1450 THEN NULL ELSE 1 END cengshen1400";
            sql += u8", CASE WHEN r.Depth<1450 OR r.Depth>=1500 THEN NULL ELSE 1 END cengshen1450";
            sql += u8", CASE WHEN r.Depth<1500 OR r.Depth>=1550 THEN NULL ELSE 1 END cengshen1500";
            sql += u8", CASE WHEN r.Depth<1550 OR r.Depth>=1600 THEN NULL ELSE 1 END cengshen1550";
            sql += u8", CASE WHEN r.Depth<1600 OR r.Depth>=1650 THEN NULL ELSE 1 END cengshen1600";
            sql += u8", CASE WHEN r.Depth<1650 OR r.Depth>=1700 THEN NULL ELSE 1 END cengshen1650";
            sql += u8", CASE WHEN r.Depth<1700 OR r.Depth>=1750 THEN NULL ELSE 1 END cengshen1700";
            sql += u8", CASE WHEN r.Depth<1750 OR r.Depth>=1800 THEN NULL ELSE 1 END cengshen1750";
            sql += u8", CASE WHEN r.Depth<1800 OR r.Depth>=1850 THEN NULL ELSE 1 END cengshen1800";
            sql += u8", CASE WHEN r.Depth<1850 OR r.Depth>=1900 THEN NULL ELSE 1 END cengshen1850";
            sql += u8", CASE WHEN r.Depth<1900 OR r.Depth>=1950 THEN NULL ELSE 1 END cengshen1900";
            sql += u8", CASE WHEN r.Depth<1950 OR r.Depth>=2000 THEN NULL ELSE 1 END cengshen1950";
            sql += u8", CASE WHEN r.Depth<2000 OR r.Depth>=2100 THEN NULL ELSE 1 END cengshen2000";
            sql += u8", CASE WHEN r.Depth<2100 OR r.Depth>=2200 THEN NULL ELSE 1 END cengshen2100";
            sql += u8", CASE WHEN r.Depth<2200 OR r.Depth>=2300 THEN NULL ELSE 1 END cengshen2200";
            sql += u8", CASE WHEN r.Depth<2300 OR r.Depth>=2400 THEN NULL ELSE 1 END cengshen2300";
            sql += u8", CASE WHEN r.Depth<2400 OR r.Depth>=2500 THEN NULL ELSE 1 END cengshen2400";
            sql += u8", CASE WHEN r.Depth<2500 OR r.Depth>=2600 THEN NULL ELSE 1 END cengshen2500";
            sql += u8", CASE WHEN r.Depth<2600 OR r.Depth>=2700 THEN NULL ELSE 1 END cengshen2600";
            sql += u8", CASE WHEN r.Depth<2700 OR r.Depth>=2800 THEN NULL ELSE 1 END cengshen2700";
            sql += u8", CASE WHEN r.Depth<2800 OR r.Depth>=2900 THEN NULL ELSE 1 END cengshen2800";
            sql += u8", CASE WHEN r.Depth<2900 OR r.Depth>=3000 THEN NULL ELSE 1 END cengshen2900";
            sql += u8", CASE WHEN r.Depth<3000 OR r.Depth>=3100 THEN NULL ELSE 1 END cengshen3000";
            sql += u8", CASE WHEN r.Depth<3100 OR r.Depth>=3200 THEN NULL ELSE 1 END cengshen3100";
            sql += u8", CASE WHEN r.Depth<3200 OR r.Depth>=3300 THEN NULL ELSE 1 END cengshen3200";
            sql += u8", CASE WHEN r.Depth<3300 OR r.Depth>=3400 THEN NULL ELSE 1 END cengshen3300";
            sql += u8", CASE WHEN r.Depth<3400 OR r.Depth>=3500 THEN NULL ELSE 1 END cengshen3400";
            sql += u8", CASE WHEN r.Depth<3500 OR r.Depth>=3600 THEN NULL ELSE 1 END cengshen3500";
            sql += u8", CASE WHEN r.Depth<3600 OR r.Depth>=3700 THEN NULL ELSE 1 END cengshen3600";
            sql += u8", CASE WHEN r.Depth<3700 OR r.Depth>=3800 THEN NULL ELSE 1 END cengshen3700";
            sql += u8", CASE WHEN r.Depth<3800 OR r.Depth>=3900 THEN NULL ELSE 1 END cengshen3800";
            sql += u8", CASE WHEN r.Depth<3900 OR r.Depth>=4000 THEN NULL ELSE 1 END cengshen3900";
            sql += u8", CASE WHEN r.Depth<4000 OR r.Depth>=4100 THEN NULL ELSE 1 END cengshen4000";
            sql += u8", CASE WHEN r.Depth<4100 OR r.Depth>=4200 THEN NULL ELSE 1 END cengshen4100";
            sql += u8", CASE WHEN r.Depth<4200 OR r.Depth>=4300 THEN NULL ELSE 1 END cengshen4200";
            sql += u8", CASE WHEN r.Depth<4300 OR r.Depth>=4400 THEN NULL ELSE 1 END cengshen4300";
            sql += u8", CASE WHEN r.Depth<4400 OR r.Depth>=4500 THEN NULL ELSE 1 END cengshen4400";
            sql += u8", CASE WHEN r.Depth<4500 OR r.Depth>=4600 THEN NULL ELSE 1 END cengshen4500";
            sql += u8", CASE WHEN r.Depth<4600 OR r.Depth>=4700 THEN NULL ELSE 1 END cengshen4600";
            sql += u8", CASE WHEN r.Depth<4700 OR r.Depth>=4800 THEN NULL ELSE 1 END cengshen4700";
            sql += u8", CASE WHEN r.Depth<4800 OR r.Depth>=4900 THEN NULL ELSE 1 END cengshen4800";
            sql += u8", CASE WHEN r.Depth<4900 OR r.Depth>=5000 THEN NULL ELSE 1 END cengshen4900";
            sql += u8", CASE WHEN r.Depth<5000 OR r.Depth>=5100 THEN NULL ELSE 1 END cengshen5000";
            sql += u8", CASE WHEN r.Depth<5100 OR r.Depth>=5200 THEN NULL ELSE 1 END cengshen5100";
            sql += u8", CASE WHEN r.Depth<5200 OR r.Depth>=5300 THEN NULL ELSE 1 END cengshen5200";
            sql += u8", CASE WHEN r.Depth<5300 OR r.Depth>=5400 THEN NULL ELSE 1 END cengshen5300";
            sql += u8", CASE WHEN r.Depth<5400 OR r.Depth>=5500 THEN NULL ELSE 1 END cengshen5400";
            sql += u8", CASE WHEN r.Depth<5500 OR r.Depth>=5600 THEN NULL ELSE 1 END cengshen5500";
            sql += u8", CASE WHEN r.Depth<5600 OR r.Depth>=5700 THEN NULL ELSE 1 END cengshen5600";
            sql += u8", CASE WHEN r.Depth<5700 OR r.Depth>=5800 THEN NULL ELSE 1 END cengshen5700";
            sql += u8", CASE WHEN r.Depth<5800 OR r.Depth>=5900 THEN NULL ELSE 1 END cengshen5800";
            sql += u8", CASE WHEN r.Depth<5900 OR r.Depth>=6000 THEN NULL ELSE 1 END cengshen5900";
            sql += u8", CASE WHEN r.Depth<6000 OR r.Depth>=6100 THEN NULL ELSE 1 END cengshen6000";
            sql += u8", CASE WHEN r.Depth<6100 OR r.Depth>=6200 THEN NULL ELSE 1 END cengshen6100";
            sql += u8", CASE WHEN r.Depth<6200 OR r.Depth>=6300 THEN NULL ELSE 1 END cengshen6200";
            sql += u8", CASE WHEN r.Depth<6300 OR r.Depth>=6400 THEN NULL ELSE 1 END cengshen6300";
            sql += u8", CASE WHEN r.Depth<6400 OR r.Depth>=6500 THEN NULL ELSE 1 END cengshen6400";
            sql += u8", CASE WHEN r.Depth<6500 OR r.Depth>=6600 THEN NULL ELSE 1 END cengshen6500";
            sql += u8", CASE WHEN r.Depth<6600 OR r.Depth>=6700 THEN NULL ELSE 1 END cengshen6600";
            sql += u8", CASE WHEN r.Depth<6700 OR r.Depth>=6800 THEN NULL ELSE 1 END cengshen6700";
            sql += u8", CASE WHEN r.Depth<6800 OR r.Depth>=6900 THEN NULL ELSE 1 END cengshen6800";
            sql += u8", CASE WHEN r.Depth<6900 OR r.Depth>=7000 THEN NULL ELSE 1 END cengshen6900";
            sql += u8", CASE WHEN r.Depth<7000 OR r.Depth>=7100 THEN NULL ELSE 1 END cengshen7000";
            sql += u8", CASE WHEN r.Depth<7100 OR r.Depth>=7200 THEN NULL ELSE 1 END cengshen7100";
            sql += u8", CASE WHEN r.Depth<7200 OR r.Depth>=7300 THEN NULL ELSE 1 END cengshen7200";
            sql += u8", CASE WHEN r.Depth<7300 OR r.Depth>=7400 THEN NULL ELSE 1 END cengshen7300";
            sql += u8", CASE WHEN r.Depth<7400 OR r.Depth>=7500 THEN NULL ELSE 1 END cengshen7400";
            sql += u8", CASE WHEN r.Depth<7500 OR r.Depth>=7600 THEN NULL ELSE 1 END cengshen7500";
            sql += u8", CASE WHEN r.Depth<7600 OR r.Depth>=7700 THEN NULL ELSE 1 END cengshen7600";
            sql += u8", CASE WHEN r.Depth<7700 OR r.Depth>=7800 THEN NULL ELSE 1 END cengshen7700";
            sql += u8", CASE WHEN r.Depth<7800 OR r.Depth>=7900 THEN NULL ELSE 1 END cengshen7800";
            sql += u8", CASE WHEN r.Depth<7900 OR r.Depth>=8000 THEN NULL ELSE 1 END cengshen7900";
            sql += u8", CASE WHEN r.Depth<8000 OR r.Depth>=8100 THEN NULL ELSE 1 END cengshen8000";
            sql += u8", CASE WHEN r.Depth<8100 OR r.Depth>=8200 THEN NULL ELSE 1 END cengshen8100";
            sql += u8", CASE WHEN r.Depth<8200 OR r.Depth>=8300 THEN NULL ELSE 1 END cengshen8200";
            sql += u8", CASE WHEN r.Depth<8300 OR r.Depth>=8400 THEN NULL ELSE 1 END cengshen8300";
            sql += u8", CASE WHEN r.Depth<8400 OR r.Depth>=8500 THEN NULL ELSE 1 END cengshen8400";
            sql += u8", CASE WHEN r.Depth<8500 OR r.Depth>=8600 THEN NULL ELSE 1 END cengshen8500";
            sql += u8", CASE WHEN r.Depth<8600 OR r.Depth>=8700 THEN NULL ELSE 1 END cengshen8600";
            sql += u8", CASE WHEN r.Depth<8700 OR r.Depth>=8800 THEN NULL ELSE 1 END cengshen8700";
            sql += u8", CASE WHEN r.Depth<8800 OR r.Depth>=8900 THEN NULL ELSE 1 END cengshen8800";
            sql += u8", CASE WHEN r.Depth<8900 OR r.Depth>=9000 THEN NULL ELSE 1 END cengshen8900";
            sql += u8", CASE WHEN r.Depth<9000 THEN NULL ELSE 1 END cengshen9000";

            sql += u8" FROM (Station s INNER JOIN Record r ON r.StationID=s.ID) INNER JOIN Cruise c ON s.Cruise_RowID=c.ID WHERE 1=1 ";
            sql += u8" AND c.SurveyProject='3003' ";

            boost::gregorian::date start_date(*year, *month, 1);
            boost::gregorian::date end_date(start_date.end_of_month());
            sql += u8" AND (s.ObsDate BETWEEN '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
            sql += u8"' AND '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
            sql += u8"')";

            if(element == 0)
            {
                sql += u8" AND r.WaterTemp<>9999";
            }
            else if(element == 1)
            {
                sql += u8" AND r.Salinity<>9999";
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

            KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER cengshen> tmp_query_result;
            while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
            {
                tmp_query_result.add(*year, *month, *element, *interval
                                     , sqlite3_column_double(sqlite_stmt, 0)
                                     , sqlite3_column_double(sqlite_stmt, 1)
                                     , sqlite3_column_int64(sqlite_stmt, 2)
                                     //
                                     , sqlite3_column_int64(sqlite_stmt, 3)
                                     , sqlite3_column_int64(sqlite_stmt, 4)
                                     , sqlite3_column_int64(sqlite_stmt, 5)
                                     , sqlite3_column_int64(sqlite_stmt, 6)
                                     , sqlite3_column_int64(sqlite_stmt, 7)
                                     , sqlite3_column_int64(sqlite_stmt, 8)
                                     , sqlite3_column_int64(sqlite_stmt, 9)
                                     , sqlite3_column_int64(sqlite_stmt, 10)
                                     , sqlite3_column_int64(sqlite_stmt, 11)
                                     , sqlite3_column_int64(sqlite_stmt, 12)
                                     , sqlite3_column_int64(sqlite_stmt, 13)
                                     , sqlite3_column_int64(sqlite_stmt, 14)
                                     , sqlite3_column_int64(sqlite_stmt, 15)
                                     , sqlite3_column_int64(sqlite_stmt, 16)
                                     , sqlite3_column_int64(sqlite_stmt, 17)
                                     , sqlite3_column_int64(sqlite_stmt, 18)
                                     , sqlite3_column_int64(sqlite_stmt, 19)
                                     , sqlite3_column_int64(sqlite_stmt, 20)
                                     , sqlite3_column_int64(sqlite_stmt, 21)
                                     , sqlite3_column_int64(sqlite_stmt, 22)
                                     , sqlite3_column_int64(sqlite_stmt, 23)
                                     , sqlite3_column_int64(sqlite_stmt, 24)
                                     , sqlite3_column_int64(sqlite_stmt, 25)
                                     , sqlite3_column_int64(sqlite_stmt, 26)
                                     , sqlite3_column_int64(sqlite_stmt, 27)
                                     , sqlite3_column_int64(sqlite_stmt, 28)
                                     , sqlite3_column_int64(sqlite_stmt, 29)
                                     , sqlite3_column_int64(sqlite_stmt, 30)
                                     , sqlite3_column_int64(sqlite_stmt, 31)
                                     , sqlite3_column_int64(sqlite_stmt, 32)
                                     , sqlite3_column_int64(sqlite_stmt, 33)
                                     , sqlite3_column_int64(sqlite_stmt, 34)
                                     , sqlite3_column_int64(sqlite_stmt, 35)
                                     , sqlite3_column_int64(sqlite_stmt, 36)
                                     , sqlite3_column_int64(sqlite_stmt, 37)
                                     , sqlite3_column_int64(sqlite_stmt, 38)
                                     , sqlite3_column_int64(sqlite_stmt, 39)
                                     , sqlite3_column_int64(sqlite_stmt, 40)
                                     , sqlite3_column_int64(sqlite_stmt, 41)
                                     , sqlite3_column_int64(sqlite_stmt, 42)
                                     , sqlite3_column_int64(sqlite_stmt, 43)
                                     , sqlite3_column_int64(sqlite_stmt, 44)
                                     , sqlite3_column_int64(sqlite_stmt, 45)
                                     , sqlite3_column_int64(sqlite_stmt, 46)
                                     , sqlite3_column_int64(sqlite_stmt, 47)
                                     , sqlite3_column_int64(sqlite_stmt, 48)
                                     , sqlite3_column_int64(sqlite_stmt, 49)
                                     , sqlite3_column_int64(sqlite_stmt, 50)
                                     , sqlite3_column_int64(sqlite_stmt, 51)
                                     , sqlite3_column_int64(sqlite_stmt, 52)
                                     , sqlite3_column_int64(sqlite_stmt, 53)
                                     , sqlite3_column_int64(sqlite_stmt, 54)
                                     , sqlite3_column_int64(sqlite_stmt, 55)
                                     , sqlite3_column_int64(sqlite_stmt, 56)
                                     , sqlite3_column_int64(sqlite_stmt, 57)
                                     , sqlite3_column_int64(sqlite_stmt, 58)
                                     , sqlite3_column_int64(sqlite_stmt, 59)
                                     , sqlite3_column_int64(sqlite_stmt, 60)
                                     , sqlite3_column_int64(sqlite_stmt, 61)
                                     , sqlite3_column_int64(sqlite_stmt, 62)
                                     , sqlite3_column_int64(sqlite_stmt, 63)
                                     , sqlite3_column_int64(sqlite_stmt, 64)
                                     , sqlite3_column_int64(sqlite_stmt, 65)
                                     , sqlite3_column_int64(sqlite_stmt, 66)
                                     , sqlite3_column_int64(sqlite_stmt, 67)
                                     , sqlite3_column_int64(sqlite_stmt, 68)
                                     , sqlite3_column_int64(sqlite_stmt, 69)
                                     , sqlite3_column_int64(sqlite_stmt, 70)
                                     , sqlite3_column_int64(sqlite_stmt, 71)
                                     , sqlite3_column_int64(sqlite_stmt, 72)
                                     , sqlite3_column_int64(sqlite_stmt, 73)
                                     , sqlite3_column_int64(sqlite_stmt, 74)
                                     , sqlite3_column_int64(sqlite_stmt, 75)
                                     , sqlite3_column_int64(sqlite_stmt, 76)
                                     , sqlite3_column_int64(sqlite_stmt, 77)
                                     , sqlite3_column_int64(sqlite_stmt, 78)
                                     , sqlite3_column_int64(sqlite_stmt, 79)
                                     , sqlite3_column_int64(sqlite_stmt, 80)
                                     , sqlite3_column_int64(sqlite_stmt, 81)
                                     , sqlite3_column_int64(sqlite_stmt, 82)
                                     , sqlite3_column_int64(sqlite_stmt, 83)
                                     , sqlite3_column_int64(sqlite_stmt, 84)
                                     , sqlite3_column_int64(sqlite_stmt, 85)
                                     , sqlite3_column_int64(sqlite_stmt, 86)
                                     , sqlite3_column_int64(sqlite_stmt, 87)
                                     , sqlite3_column_int64(sqlite_stmt, 88)
                                     , sqlite3_column_int64(sqlite_stmt, 89)
                                     , sqlite3_column_int64(sqlite_stmt, 90)
                                     , sqlite3_column_int64(sqlite_stmt, 91)
                                     , sqlite3_column_int64(sqlite_stmt, 92)
                                     , sqlite3_column_int64(sqlite_stmt, 93)
                                     , sqlite3_column_int64(sqlite_stmt, 94)
                                     , sqlite3_column_int64(sqlite_stmt, 95)
                                     , sqlite3_column_int64(sqlite_stmt, 96)
                                     , sqlite3_column_int64(sqlite_stmt, 97)
                                     , sqlite3_column_int64(sqlite_stmt, 98)
                                     , sqlite3_column_int64(sqlite_stmt, 99)
                                     , sqlite3_column_int64(sqlite_stmt, 100)
                                     , sqlite3_column_int64(sqlite_stmt, 101)
                                     , sqlite3_column_int64(sqlite_stmt, 102)
                                     , sqlite3_column_int64(sqlite_stmt, 103)
                                     , sqlite3_column_int64(sqlite_stmt, 104)
                                     , sqlite3_column_int64(sqlite_stmt, 105)
                                     , sqlite3_column_int64(sqlite_stmt, 106)
                                     , sqlite3_column_int64(sqlite_stmt, 107)
                                     , sqlite3_column_int64(sqlite_stmt, 108)
                                     , sqlite3_column_int64(sqlite_stmt, 109)
                                     , sqlite3_column_int64(sqlite_stmt, 110)
                                     , sqlite3_column_int64(sqlite_stmt, 111)
                                     , sqlite3_column_int64(sqlite_stmt, 112)
                                     , sqlite3_column_int64(sqlite_stmt, 113)
                                     , sqlite3_column_int64(sqlite_stmt, 114)
                                     , sqlite3_column_int64(sqlite_stmt, 115)
                                     , sqlite3_column_int64(sqlite_stmt, 116)
                                     , sqlite3_column_int64(sqlite_stmt, 117)
                                     , sqlite3_column_int64(sqlite_stmt, 118)
                                     , sqlite3_column_int64(sqlite_stmt, 119)
                                     , sqlite3_column_int64(sqlite_stmt, 120)
                                     , sqlite3_column_int64(sqlite_stmt, 121)
                                     , sqlite3_column_int64(sqlite_stmt, 122)
                                     , sqlite3_column_int64(sqlite_stmt, 123)
                                     , sqlite3_column_int64(sqlite_stmt, 124)
                                     , sqlite3_column_int64(sqlite_stmt, 125)
                                     , sqlite3_column_int64(sqlite_stmt, 126)
                                     , sqlite3_column_int64(sqlite_stmt, 127)
                                     , sqlite3_column_int64(sqlite_stmt, 128)
                                     , sqlite3_column_int64(sqlite_stmt, 129)
                                     , sqlite3_column_int64(sqlite_stmt, 130)
                                     , sqlite3_column_int64(sqlite_stmt, 131)
                                     , sqlite3_column_int64(sqlite_stmt, 132)
                                     , sqlite3_column_int64(sqlite_stmt, 133)
                                     , sqlite3_column_int64(sqlite_stmt, 134)
                                     , sqlite3_column_int64(sqlite_stmt, 135)
                                     , sqlite3_column_int64(sqlite_stmt, 136)
                                     , sqlite3_column_int64(sqlite_stmt, 137)
                                     , sqlite3_column_int64(sqlite_stmt, 138)
                                     , sqlite3_column_int64(sqlite_stmt, 139)
                                     );
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

        if(query_result_.size() > 0)
        {
            //通过服务器上传数据query_result_

            KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

            send_http_post_request_.set_target(u8"/cengshen/query_result");

            boost::property_tree::wptree tree;
            query_result_.to_property_tree(tree);
            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);
            send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

            if(!send_http_post_request_())
            {
                BOOST_LOG_TRIVIAL(error) << "update error";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送结果请求失败");
            }
        }

        BOOST_LOG_TRIVIAL(error) << "update finished";
    }
    else if(name == KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"tongcun"))
    {
        BOOST_LOG_TRIVIAL(error) << "sqlite file count:" << (*sqlite_filepath).size();

        //////
        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongcun> query_result_;
        for(auto const& filepath : *sqlite_filepath)
        {
            BOOST_LOG_TRIVIAL(error) << "sqlite file path:" << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(filepath).c_str();

            boost::filesystem::path sqlite_filepath_path(filepath);

            if(!boost::filesystem::exists(sqlite_filepath_path))
            {
                continue;
            }

            sqlite3* pDB = nullptr;
            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(sqlite_filepath_path.native()).c_str()
                    , &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
            if(open_result != SQLITE_OK)
            {
                BOOST_LOG_TRIVIAL(error) << "打开数据库连接失败";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"打开数据库连接失败：") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(open_result)));
            }

            std::u8string sql = u8"SELECT subtable.Lat, subtable.Lon, count(subtable.TongCun) TongCunCount, count(*) StationCount FROM ";

            sql += u8"(";
            sql += u8"SELECT ";

            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                }
            }
            sql += u8" END Lat";
            sql += u8",";
            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                }
            }
            sql += u8" END Lon";
            sql += u8", CASE WHEN r.WaterTemp=9999 OR r.Salinity=9999 THEN NULL ELSE 1 END TongCun";

            sql += u8" FROM (Station s INNER JOIN Record r ON r.StationID=s.ID) INNER JOIN Cruise c ON s.Cruise_RowID=c.ID WHERE 1=1 ";
            sql += u8" AND c.SurveyProject='3003' ";

            boost::gregorian::date start_date(*year, *month, 1);
            boost::gregorian::date end_date(start_date.end_of_month());
            sql += u8" AND (s.ObsDate BETWEEN '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
            sql += u8"' AND '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
            sql += u8"')";

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

            KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongcun> tmp_query_result;
            while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
            {
                tmp_query_result.add(*year, *month, *interval, sqlite3_column_double(sqlite_stmt, 0)
                                     , sqlite3_column_double(sqlite_stmt, 1)
                                     , sqlite3_column_int64(sqlite_stmt, 2)
                                     , sqlite3_column_int64(sqlite_stmt, 3));
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

        if(query_result_.size() > 0)
        {
            //通过服务器上传数据query_result_

            KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

            send_http_post_request_.set_target(u8"/tongcun/query_result");

            boost::property_tree::wptree tree;
            query_result_.to_property_tree(tree);
            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);
            send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

            if(!send_http_post_request_())
            {
                BOOST_LOG_TRIVIAL(error) << "update error";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送结果请求失败");
            }
        }

        BOOST_LOG_TRIVIAL(error) << "update finished";
    }
    else if(name == KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"chongfu"))
    {
        BOOST_LOG_TRIVIAL(error) << "sqlite file count:" << (*sqlite_filepath).size();

        //////
        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER chongfu> query_result_;
        for(auto const& filepath : *sqlite_filepath)
        {
            BOOST_LOG_TRIVIAL(error) << "sqlite file path:" << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(filepath).c_str();

            boost::filesystem::path sqlite_filepath_path(filepath);

            if(!boost::filesystem::exists(sqlite_filepath_path))
            {
                continue;
            }

            sqlite3* pDB = nullptr;
            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(sqlite_filepath_path.native()).c_str()
                    , &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
            if(open_result != SQLITE_OK)
            {
                BOOST_LOG_TRIVIAL(error) << "打开数据库连接失败";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"打开数据库连接失败：") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(open_result)));
            }

            std::u8string sql = u8"SELECT subtable.Lat, subtable.Lon, count(subtable.RepeatIsNotNull) RepeatCount, count(*) AllCount FROM ";

            sql += u8"(";
            sql += u8"SELECT ";

            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                }
            }
            sql += u8" END Lat";
            sql += u8",";
            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                }
            }
            sql += u8" END Lon";
            sql += u8",";
            sql += u8" CASE WHEN s.DistinctTag IS NULL AND s.RepeatTag IS NULL THEN NULL ELSE 1 END RepeatIsNotNull";
            sql += u8" FROM (Station s INNER JOIN Record r ON r.StationID=s.ID) INNER JOIN Cruise c ON s.Cruise_RowID=c.ID WHERE 1=1 ";
            sql += u8" AND c.SurveyProject='3003' ";

            boost::gregorian::date start_date(*year, *month, 1);
            boost::gregorian::date end_date(start_date.end_of_month());
            sql += u8" AND (s.ObsDate BETWEEN '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
            sql += u8"' AND '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
            sql += u8"')";

            if(element == 0)
            {
                sql += u8" AND r.WaterTemp<>9999";
            }
            else if(element == 1)
            {
                sql += u8" AND r.Salinity<>9999";
            }

            sql += u8" AND s.LatLonQuaTag=0";

            sql += u8") subtable";

            sql += u8" GROUP BY subtable.Lat, subtable.Lon";

            sql += u8" HAVING COUNT(subtable.RepeatIsNotNull)>0;";

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

            KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER chongfu> tmp_query_result;
            while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
            {
                tmp_query_result.add(*year, *month, *element, *interval
                                     , sqlite3_column_double(sqlite_stmt, 0)
                                     , sqlite3_column_double(sqlite_stmt, 1)
                                     , sqlite3_column_int64(sqlite_stmt, 2)
                                     , sqlite3_column_int64(sqlite_stmt, 3)
                                     );
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

        if(query_result_.size() > 0)
        {
            //通过服务器上传数据query_result_

            KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

            send_http_post_request_.set_target(u8"/chongfu/query_result");

            boost::property_tree::wptree tree;
            query_result_.to_property_tree(tree);
            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);
            send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

            if(!send_http_post_request_())
            {
                BOOST_LOG_TRIVIAL(error) << "update error";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送结果请求失败");
            }
        }

        BOOST_LOG_TRIVIAL(error) << "update finished";
    }
    else if(name == KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"tongguo"))
    {
        BOOST_LOG_TRIVIAL(error) << "sqlite file count:" << (*sqlite_filepath).size();

        //////
        KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongguo> query_result_;
        for(auto const& filepath : *sqlite_filepath)
        {
            BOOST_LOG_TRIVIAL(error) << "sqlite file path:" << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(filepath).c_str();

            boost::filesystem::path sqlite_filepath_path(filepath);

            if(!boost::filesystem::exists(sqlite_filepath_path))
            {
                continue;
            }

            sqlite3* pDB = nullptr;
            int open_result = sqlite3_open_v2(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_utf8_string(sqlite_filepath_path.native()).c_str()
                    , &pDB, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX, NULL);
            if(open_result != SQLITE_OK)
            {
                BOOST_LOG_TRIVIAL(error) << "打开数据库连接失败";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"打开数据库连接失败：") + KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(open_result)));
            }

            std::u8string sql = u8"SELECT subtable.Lat, subtable.Lon, subtable.InstrumentType";

            sql += u8",count(LatLonRangeIsNotNull) LatLonRangeCount";
            sql += u8",count(LandingDiscriminationIsNotNull) LandingDiscriminationCount";
            sql += u8",count(TimeIsNotNull) TimeCount";
            sql += u8",count(StandardLayerDensityStabilityIsNotNull) StandardLayerDensityStabilityCount";
            sql += u8",count(StandardLayerTemeratureSalinityStatisticsIsNotNull) StandardLayerTemperatureSalinityStatisticsCount";
            sql += u8",count(MeasuredLayerDensityStabilityIsNotNull) MeasuredLayerDensityStabilityCount";
            sql += u8",count(TemperatureSalinityGradientIsNotNull) TemperatureSalinityGradientCount";
            sql += u8",count(IncrementalIsNotNull) IncrementalCount";
            sql += u8",count(IllegalCodeIsNotNull) IllegalCodeCount";
            sql += u8",count(LevelReversalRepetitionIsNotNull) LevelReversalRepetitionCount";
            sql += u8",count(ExtremumIsNotNull) ExtremumCount";
            sql += u8",count(*) AllCount";

            sql += u8" FROM ";

            sql += u8"(";
            sql += u8"SELECT ";

            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                }
            }
            sql += u8" END Lat";
            sql += u8",";
            sql += u8" CASE";
            for(int lat = 90; lat >= -90; lat -= *interval)
            {
                for(int lon = -180; lon <= 180; lon += *interval)
                {
                    sql += u8" WHEN s.Lat BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat - *interval));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lat));
                    sql += u8" AND s.Lon BETWEEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                    sql += u8" AND ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon + *interval));
                    sql += u8" THEN ";
                    sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(std::to_wstring(lon));
                }
            }
            sql += u8" END Lon";

            sql += u8", s.InstrumentType";
            sql += u8", CASE s.LatLonQuaTag WHEN 0 THEN NULL WHEN 1 THEN 1 END LatLonRangeIsNotNull";
            sql += u8", CASE s.LatLonQuaTag WHEN 0 THEN NULL WHEN 2 THEN 1 END LandingDiscriminationIsNotNull";
            sql += u8", CASE s.DateQuaTag WHEN 0 THEN NULL WHEN 3 THEN 1 END TimeIsNotNull";
            if(element == 0)
            {
                sql += u8", CASE r.WaterTempQuaTag WHEN 0 THEN NULL WHEN 1 THEN 1 END StandardLayerDensityStabilityIsNotNull";
                sql += u8", CASE r.WaterTempQuaTag WHEN 0 THEN NULL WHEN 2 THEN 1 END StandardLayerTemperatureSalinityStatisticsIsNotNull";
                sql += u8", CASE r.WaterTempQuaTag WHEN 0 THEN NULL WHEN 3 THEN 1 END MeasuredLayerDensityStabilityIsNotNull";
                sql += u8", CASE r.WaterTempQuaTag WHEN 0 THEN NULL WHEN 4 THEN 1 END TemperatureSalinityGradientIsNotNull";
                sql += u8", CASE r.WaterTempQuaTag WHEN 0 THEN NULL WHEN 5 THEN 1 END IncrementalIsNotNull";
                sql += u8", CASE r.WaterTempQuaTag WHEN 0 THEN NULL WHEN 6 THEN 1 END IllegalCodeIsNotNull";
                sql += u8", CASE r.WaterTempQuaTag WHEN 0 THEN NULL WHEN 7 THEN 1 END LevelReversalRepetitionIsNotNull";
                sql += u8", CASE r.WaterTempQuaTag WHEN 0 THEN NULL WHEN 8 THEN 1 END ExtremumIsNotNull";
            }
            else if(element == 1)
            {
                sql += u8", CASE r.SalinityQuaTag WHEN 0 THEN NULL WHEN 1 THEN 1 END StandardLayerDensityStabilityIsNotNull";
                sql += u8", CASE r.SalinityQuaTag WHEN 0 THEN NULL WHEN 2 THEN 1 END StandardLayerTemperatureSalinityStatisticsIsNotNull";
                sql += u8", CASE r.SalinityQuaTag WHEN 0 THEN NULL WHEN 3 THEN 1 END MeasuredLayerDensityStabilityIsNotNull";
                sql += u8", CASE r.SalinityQuaTag WHEN 0 THEN NULL WHEN 4 THEN 1 END TemperatureSalinityGradientIsNotNull";
                sql += u8", CASE r.SalinityQuaTag WHEN 0 THEN NULL WHEN 5 THEN 1 END IncrementalIsNotNull";
                sql += u8", CASE r.SalinityQuaTag WHEN 0 THEN NULL WHEN 6 THEN 1 END IllegalCodeIsNotNull";
                sql += u8", CASE r.SalinityQuaTag WHEN 0 THEN NULL WHEN 7 THEN 1 END LevelReversalRepetitionIsNotNull";
                sql += u8", CASE r.SalinityQuaTag WHEN 0 THEN NULL WHEN 8 THEN 1 END ExtremumIsNotNull";
            }

            sql += u8" FROM (Station s INNER JOIN Record r ON r.StationID=s.ID) INNER JOIN Cruise c ON s.Cruise_RowID=c.ID WHERE 1=1 ";
            sql += u8" AND c.SurveyProject='3003' ";

            boost::gregorian::date start_date(*year, *month, 1);
            boost::gregorian::date end_date(start_date.end_of_month());
            sql += u8" AND (s.ObsDate BETWEEN '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(start_date));
            sql += u8"' AND '";
            sql += KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(boost::gregorian::to_sql_string(end_date));
            sql += u8"')";

            if(element == 0)
            {
                sql += u8" AND r.WaterTemp<>9999";
            }
            else if(element == 1)
            {
                sql += u8" AND r.Salinity<>9999";
            }

            sql += u8") subtable";

            sql += u8" GROUP BY subtable.Lat, subtable.Lon, subtable.InstrumentType;";

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

            KIWI_FAST_MODEL_NAMESPACE_QUALIFIER query_result<KIWI_FAST_MODEL_NAMESPACE_QUALIFIER tongguo> tmp_query_result;
            while(sqlite3_step(sqlite_stmt) == SQLITE_ROW)
            {
                tmp_query_result.add(*year, *month, *element, *interval, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(sqlite3_column_text(sqlite_stmt, 2))
                                     , sqlite3_column_double(sqlite_stmt, 0)
                                     , sqlite3_column_double(sqlite_stmt, 1)
                                     , sqlite3_column_int64(sqlite_stmt, 3)
                                     , sqlite3_column_int64(sqlite_stmt, 4)
                                     , sqlite3_column_int64(sqlite_stmt, 5)
                                     , sqlite3_column_int64(sqlite_stmt, 6)
                                     , sqlite3_column_int64(sqlite_stmt, 7)
                                     , sqlite3_column_int64(sqlite_stmt, 8)
                                     , sqlite3_column_int64(sqlite_stmt, 9)
                                     , sqlite3_column_int64(sqlite_stmt, 10)
                                     , sqlite3_column_int64(sqlite_stmt, 11)
                                     , sqlite3_column_int64(sqlite_stmt, 12)
                                     , sqlite3_column_int64(sqlite_stmt, 13)
                                     , sqlite3_column_int64(sqlite_stmt, 14)
                                     );
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

        if(query_result_.size() > 0)
        {
            //通过服务器上传数据query_result_

            KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER send_http_post_request<boost::beast::http::basic_string_body<char8_t>> send_http_post_request_(u8"127.0.0.1", u8"8911");

            send_http_post_request_.set_target(u8"/tongguo/query_result");

            boost::property_tree::wptree tree;
            query_result_.to_property_tree(tree);
            std::wstringstream wsstream;
            boost::property_tree::write_xml(wsstream, tree);
            send_http_post_request_.set_body(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(wsstream.str()));

            if(!send_http_post_request_())
            {
                BOOST_LOG_TRIVIAL(error) << "update error";

                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"发送结果请求失败");
            }
        }

        BOOST_LOG_TRIVIAL(error) << "update finished";
    }

    return 0;
}
