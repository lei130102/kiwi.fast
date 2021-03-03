#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

//错误码机制和异常机制是两套东西，如果提供错误码那么禁止抛出异常

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

using error_code = unsigned int;

namespace error_code_value
{
    inline error_code const ok                                         = 0;  //正常
    inline error_code const exception                                  = 1;  //抛出异常
    inline error_code const request_send_error                         = 2;  //请求发送失败
    //...
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
