#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE
class service_log;
KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

TYPE_CONVERTER_TO_U8STRING_NO_DEQUE(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_log, u8"service_log")

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

/*!
 * \brief The service_log class
 * 日志
 */
class service_log
{
protected:
    virtual ~service_log(){}

public:

};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
