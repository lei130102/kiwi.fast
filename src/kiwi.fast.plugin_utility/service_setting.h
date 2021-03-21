#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
//#include <kiwi.fast.plugin_utility/setting_visitor.h>

#include <string>
#include <any>
#include <optional>
#include <filesystem>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

template<typename Setting>
class service_visitor;

/*!
 * \brief The service_setting class
 * service<service_setting> setting_service;
 * 获取配置信息
 * std::optional<unsigned int*> r = setting_service->setting(u8"background")(u8"r").value();   //不带有默认值
 * unsigned int* r = setting_service->setting(u8"background")(u8"r").value(255);               //带有默认值
 * 修改配置信息
 * bool is_success = setting_service->setting(u8"background")(u8"r").set_value(24);
 * 同理，临时配置用tmp_setting
 */
class service_setting
{
    friend class service_visitor<service_setting>;

protected:
    virtual ~service_setting(){}

public:
    ////virtual ptree_root& setting_default() = 0;
    ////virtual ptree_root& setting_user() = 0;
    ////virtual ptree_root& setting_tmp() = 0;

    ////template<typename CharType>
    ////setting_visitor<service_setting> setting(CharType const* name)
    ////{
    ////    setting_visitor<service_setting> visitor(this);
    ////    visitor.add_name(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name));
    ////    return visitor;
    ////}

    ////template<typename CharType>
    ////setting_visitor<service_setting> tmp_setting(CharType const* name)
    ////{
    ////    setting_visitor<service_setting> visitor(this);
    ////    visitor.set_tmp(true);
    ////    visitor.add_name(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name));
    ////    return visitor;
    ////}

    //////常用的列在这里
    ////virtual std::optional<std::filesystem::path*> bin_dir_path() = 0;
    ////virtual std::optional<std::filesystem::path*> root_dir_path() = 0;
    ////virtual std::optional<std::filesystem::path*> default_setting_dir_path() = 0;
    ////virtual std::optional<std::filesystem::path*> temp_dir_path() = 0;
    ////virtual std::optional<std::filesystem::path*> plugin_dir_path() = 0;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

TYPE_CONVERTER_TO_U8STRING_NO_DEQUE(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_setting, u8"service_setting")
