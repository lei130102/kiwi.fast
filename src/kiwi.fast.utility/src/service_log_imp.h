#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>

#define BOOST_LOG_DYN_LINK 1
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_log_imp
{
public:

protected:
    service_log_imp()
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
    }

    virtual ~service_log_imp()
    {}

private:

};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE

#define SERVICE_LOG_ADAPTER_METHOD(imp_class)
