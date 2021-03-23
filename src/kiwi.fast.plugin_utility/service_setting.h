#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/ptree_item.h>
#include <kiwi.fast.plugin_utility/setting_visitor.h>

#include <string>
#include <any>
#include <optional>
#include <filesystem>
#include <memory>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

/*!
 * \brief The service_setting class
 * service<service_setting> setting_service;
 * 获取缺省配置信息
 * std::optional<unsigned int*> r = setting_service->default_setting(u8"background")(u8"r").value();   //不带有默认值
 * unsigned int* r = setting_service->setting(u8"background")(u8"r").value(255);               //带有默认值
 * 修改配置信息
 * bool is_success = setting_service->setting(u8"background")(u8"r").set_value(24);
 * 同理，临时配置用tmp_setting
 */
class service_setting
{
protected:
    virtual ~service_setting(){}

public:
    virtual std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_default() = 0;
    virtual std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_user() = 0;
    virtual std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_tmp() = 0;

    template<typename CharType>
    std::shared_ptr<setting_visitor> default_setting(CharType const* name)
    {
        return default_setting(std::basic_string<CharType>(name));
    }
    template<typename CharType>
    std::shared_ptr<setting_visitor> default_setting(std::basic_string<CharType> const& name)
    {
        return default_setting(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str());
    }
    std::shared_ptr<setting_visitor> default_setting(const char8_t* name)
    {
        auto ret = std::make_shared<setting_visitor>(setting_default().get());
        ret->set_name(name);
        return ret;
    }

    template<typename CharType>
    std::shared_ptr<setting_visitor> user_setting(CharType const* name)
    {
        return user_setting(std::basic_string<CharType>(name));
    }
    template<typename CharType>
    std::shared_ptr<setting_visitor> user_setting(std::basic_string<CharType> const& name)
    {
        return user_setting(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str());
    }
    std::shared_ptr<setting_visitor> user_setting(const char8_t* name)
    {
        auto ret = std::make_shared<setting_visitor>(setting_user().get());
        ret->set_name(name);
        return ret;
    }

    template<typename CharType>
    std::shared_ptr<setting_visitor> tmp_setting(CharType const* name)
    {
        return tmp_setting(std::basic_string<CharType>(name));
    }
    template<typename CharType>
    std::shared_ptr<setting_visitor> tmp_setting(std::basic_string<CharType> const& name)
    {
        return tmp_setting(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str());
    }
    std::shared_ptr<setting_visitor> tmp_setting(const char8_t* name)
    {
        auto ret = std::make_shared<setting_visitor>(setting_tmp().get());
        ret->set_name(name);
        return ret;
    }



protected:
    //该函数仅获取根子节点配置的值，所以protected，不公开
    template<typename T>
    std::optional<T> get_setting(std::u8string const& name)
    {
        auto value = tmp_setting(name)->value();
        if (!value)
        {
            value = user_setting(name)->value();
            if (!value)
            {
                value = default_setting(name)->value();
                if (!value)
                {
                    return {};
                }
            }
        }

        return (*value).get().value<T>();
    }

public:

    //常用的只读配置

    std::optional<std::filesystem::path> bin_dir_path()
    {
        return get_setting<std::filesystem::path>(u8"bin_dir_path");
    }

    std::optional<std::filesystem::path> root_dir_path()
    {
        return get_setting<std::filesystem::path>(u8"root_dir_path");
    }

    std::optional<std::filesystem::path> default_setting_dir_path()
    {
        return get_setting<std::filesystem::path>(u8"default_setting_dir_path");
    }

    std::optional<std::filesystem::path> temp_dir_path()
    {
        return get_setting<std::filesystem::path>(u8"temp_dir_path");
    }

    std::optional<std::filesystem::path> plugin_dir_path()
    {
        return get_setting<std::filesystem::path>(u8"plugin_dir_path");
    }

    std::optional<std::filesystem::path> user_dir_path()
    {
        return get_setting<std::filesystem::path>(u8"user_dir_path");
    }
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

TYPE_CONVERTER_TO_U8STRING_NO_DEQUE(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_setting, u8"service_setting")
