#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE
class service_exe_run_mode;
KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

TYPE_CONVERTER_TO_U8STRING_NO_DEQUE(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_exe_run_mode, u8"service_exe_run_mode")

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

/*!
 * \brief The service_exe_run_mode class
 * exe程序运行模式，比如指定交互式模式或者批处理模式等
 */
class service_exe_run_mode
{
protected:
    virtual ~service_exe_run_mode(){}

public:
    /*!
     * \brief set_interactive
     * 设置交互式运行模式
     */
    virtual void set_interactive() = 0;

    /*!
     * \brief interactive
     * \return
     */
    virtual bool interactive() = 0;

    /*!
     * \brief set_batch
     * 设置批处理运行模式
     */
    virtual void set_batch() = 0;

    /*!
     * \brief batch
     * \return
     */
    virtual bool batch() = 0;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
