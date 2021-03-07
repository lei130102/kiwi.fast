#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>

#include <string>
#include <any>
#include <optional>
#include <filesystem>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class service_setting
{
protected:
    virtual ~service_setting(){}

public:
    /*!
     *
     */
    template<typename T, typename CharType>
    T const* setting(CharType const* name) const
    {
        return setting(std::basic_string<CharType>(name));
    }
    template<typename T, typename CharType>
    T const* setting(std::basic_string<CharType> const& name) const
    {
        std::optional<std::any> value = setting(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str());
        if(value)
        {
            try {
                return &std::any_cast<T const&>(*value);
            }  catch (std::bad_any_cast& e) {
                return nullptr;
            }
        }
        else
        {
            return nullptr;
        }
    }
    virtual std::optional<std::any> setting(const char8_t* name) const = 0;

    /*!
     *
     */
    template<typename T, typename CharType = char8_t>
    void set_setting(CharType const* name, CharType const* type, T const& value)
    {
        set_setting(std::basic_string<CharType>(name)
                    , std::basic_string<CharType>(type)
                    , value);
    }
    template<typename T, typename CharType = char8_t>
    void set_setting(std::basic_string<CharType> const& name, std::basic_string<CharType> const& type, T const& value)
    {
        set_setting(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
                    , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(type).c_str()
                    , value);
    }
    virtual void set_setting(const char8_t* name, const char8_t* type, std::any const& value) = 0;

    /*!
     *
     */
    template<typename T, typename CharType = char8_t>
    void set_tmp_setting(CharType const* name, CharType const* type, T const& value)
    {
        set_tmp_setting(std::basic_string<CharType>(name)
                    , std::basic_string<CharType>(type)
                    , value);
    }
    template<typename T, typename CharType = char8_t>
    void set_tmp_setting(std::basic_string<CharType> const& name, std::basic_string<CharType> const& type, T const& value)
    {
        set_tmp_setting(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name).c_str()
                    , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(type).c_str()
                    , value);
    }
    virtual void set_tmp_setting(const char8_t* name, const char8_t* type, std::any const& value) = 0;

    virtual std::optional<std::filesystem::path> bin_dir_path() const = 0;
    virtual std::optional<std::filesystem::path> root_dir_path() const = 0;
    virtual std::optional<std::filesystem::path> default_setting_dir_path() const = 0;
    virtual std::optional<std::filesystem::path> temp_dir_path() const = 0;
    virtual std::optional<std::filesystem::path> plugin_dir_path() const = 0;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
