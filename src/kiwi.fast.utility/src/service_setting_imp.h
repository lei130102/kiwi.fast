#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/manager_module.h>

#include <kiwi.fast.utility/src/service_object_factory_adapter.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <map>
#include <string>
#include <any>
#include <filesystem>
#include <optional>
#include <algorithm>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_setting_imp
{
public:
    /*!
     * \brief The setting_item_type struct
     * 配置项
     */
    struct setting_item_type
    {
        std::u8string name;//名称
        std::u8string type;//类型
        std::any value;//值
    };

    /*!
     * 配置
     */
    using setting_type = std::map<std::u8string, setting_item_type>;//名称 配置项

    using setting_index_type = int;
    using setting_path_type = std::filesystem::path;

    std::optional<std::any> setting(const char8_t* name) const
    {
        auto iter = m_setting_tmp.find(name);
        if(iter != m_setting_tmp.end())
        {
            //临时配置
            return iter->second.value;
        }
        else
        {
            iter = m_setting_user.find(name);
            if(iter != m_setting_user.end())
            {
                //用户配置
                return iter->second.value;
            }
            else
            {
                iter = m_setting_default.find(name);
                if(iter != m_setting_default.end())
                {
                    //缺省配置
                    return iter->second.value;
                }
            }
        }
        return {};
    }

    void set_setting(const char8_t* name, const char8_t* type, std::any const& value)
    {
        m_setting_user.insert(setting_type::value_type(name,
                                                       setting_item_type{name
                                                                        ,type
                                                                        ,value}));
    }

    void set_tmp_setting(const char8_t* name, const char8_t* type, std::any const& value)
    {
        m_setting_tmp.insert(setting_type::value_type(name,
                                                       setting_item_type{name
                                                                        ,type
                                                                        ,value}));
    }

    std::optional<std::filesystem::path> bin_dir_path() const
    {
        auto value = setting(u8"bin_dir_path");
        if(value)
        {
            try {
                return std::any_cast<std::filesystem::path const&>(*value);
            }  catch (std::bad_any_cast& e) {
            }
        }
        return {};
    }

    std::optional<std::filesystem::path> root_dir_path() const
    {
        auto value = setting(u8"root_dir_path");
        if(value)
        {
            try {
                return std::any_cast<std::filesystem::path const&>(*value);
            }  catch (std::bad_any_cast& e) {
            }
        }
        return {};
    }

    std::optional<std::filesystem::path> default_setting_dir_path() const
    {
        auto value = setting(u8"default_setting_dir_path");
        if(value)
        {
            try {
                return std::any_cast<std::filesystem::path const&>(*value);
            }  catch (std::bad_any_cast& e) {
            }
        }
        return {};
    }

    std::optional<std::filesystem::path> temp_dir_path() const
    {
        auto value = setting(u8"temp_dir_path");
        if(value)
        {
            try {
                return std::any_cast<std::filesystem::path const&>(*value);
            }  catch (std::bad_any_cast& e) {
            }
        }
        return {};
    }

    std::optional<std::filesystem::path> plugin_dir_path() const
    {
        auto value = setting(u8"plugin_dir_path");
        if(value)
        {
            try {
                return std::any_cast<std::filesystem::path const&>(*value);
            }  catch (std::bad_any_cast& e) {
            }
        }
        return {};
    }

protected:
    service_setting_imp()
    {
        init_path();

        load_default_setting();
    }

    virtual ~service_setting_imp()
    {}

    /*!
     * \brief init_path
     * 初始化路径信息
     */
    void init_path()
    {
        std::filesystem::path bin_dir_path = std::filesystem::current_path();
        {
            setting_item_type value{u8"bin_dir_path"
                              , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::filesystem::path>()
                              , std::any(bin_dir_path) };
            m_setting_default.insert(setting_type::value_type(value.name, value));
        }

        std::filesystem::path root_dir_path = bin_dir_path.parent_path();
        {
            setting_item_type value{u8"root_dir_path"
                              , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::filesystem::path>()
                              , std::any(root_dir_path) };
            m_setting_default.insert(setting_type::value_type(value.name, value));
        }

        std::filesystem::path default_setting_dir_path = root_dir_path / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"default_setting"));
        {
            setting_item_type value{u8"default_setting_dir_path"
                              , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::filesystem::path>()
                              , std::any(default_setting_dir_path) };
            m_setting_default.insert(setting_type::value_type(value.name, value));
        }

        std::filesystem::path temp_dir_path = root_dir_path / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"temp"));
        {
            setting_item_type value{u8"temp_dir_path"
                              , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::filesystem::path>()
                              , std::any(temp_dir_path) };
            m_setting_default.insert(setting_type::value_type(value.name, value));
        }

        std::filesystem::path plugin_dir_path = root_dir_path / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"plugin"));
        {
            setting_item_type value{u8"plugin_dir_path"
                              , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::filesystem::path>()
                              , std::any(plugin_dir_path) };
            m_setting_default.insert(setting_type::value_type(value.name, value));
        }
    }

    struct setting_index_less
    {
        bool operator()(std::pair<setting_index_type, std::wstring> const& lhs, std::pair<setting_index_type, std::wstring> const& rhs)
        {
            if(lhs.first < rhs.first)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    };

    /*!
     * \brief parse_setting_file
     * 解析配置文件
     * \param file_info
     * \param setting
     */
    void parse_setting_file(std::pair<setting_index_type, setting_path_type> const& file_info, setting_type& setting)
    {
        service_object_factory_imp* service_object_factory_imp_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(
                    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory>()
                    , reinterpret_cast<void**>(&service_object_factory_imp_));
        if(!service_object_factory_imp_)
        {
            return;
        }

        boost::property_tree::wptree tree;
        boost::property_tree::read_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(file_info.second.native()), tree);

        for(auto const& v : tree.get_child(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"kiwi_fast.item")))
        {
            std::u8string name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(v.second.get<std::wstring>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"name")));
            std::u8string type = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(v.second.get<std::wstring>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"type")));
            std::u8string value = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(v.second.get<std::wstring>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"value")));

            //service_object_factory_imp_->create_object(type.c_str());
        }
    }

    /*!
     * \brief load_default_setting
     * 加载缺省配置
     */
    void load_default_setting()
    {
        std::deque<std::pair<setting_index_type, setting_path_type>> default_setting_file_info;

        auto default_setting_dir = default_setting_dir_path();
        if(default_setting_dir)
        {
            std::filesystem::directory_iterator end;
            for(std::filesystem::directory_iterator iter(*default_setting_dir); iter != end; ++iter)
            {
                if(std::filesystem::is_regular_file(*iter))
                {
                    //日志输出
                    //std::wstring setting_path = iter->path().native();

                    boost::property_tree::wptree tree;
                    boost::property_tree::read_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(iter->path().native()), tree);

                    setting_index_type setting_index = tree.get<setting_index_type>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"kiwi_fast.index"), 999);
                    default_setting_file_info.push_back(std::make_pair(setting_index, iter->path()));
                }
            }
        }

        //排序
        std::sort(default_setting_file_info.begin(), default_setting_file_info.end(), setting_index_less());

        //
        std::for_each(default_setting_file_info.begin(), default_setting_file_info.end()
                      , [&](std::pair<setting_index_type, setting_path_type> const& file_info){
            parse_setting_file(file_info, m_setting_default);
        });
    }


private:
    setting_type m_setting_default;      //缺省配置
    setting_type m_setting_user;         //用户配置
    setting_type m_setting_tmp;          //临时配置
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE

#define SERVICE_SETTING_ADAPTER_METHOD(imp_class)                                                              \
    std::optional<std::any> setting(const char8_t* name) const                                                 \
    {                                                                                                          \
        return imp_class::setting(name);                                                                       \
    }                                                                                                          \
    void set_setting(const char8_t* name, const char8_t* type, std::any const& value)                          \
    {                                                                                                          \
        imp_class::set_setting(name, type, value);                                                             \
    }                                                                                                          \
    void set_tmp_setting(const char8_t* name, const char8_t* type, std::any const& value)                      \
    {                                                                                                          \
        imp_class::set_tmp_setting(name, type, value);                                                         \
    }                                                                                                          \
    std::optional<std::filesystem::path> bin_dir_path() const                                                  \
    {                                                                                                          \
        return imp_class::bin_dir_path();                                                                      \
    }                                                                                                          \
    std::optional<std::filesystem::path> root_dir_path() const                                                 \
    {                                                                                                          \
        return imp_class::root_dir_path();                                                                     \
    }                                                                                                          \
    std::optional<std::filesystem::path> default_setting_dir_path() const                                      \
    {                                                                                                          \
        return imp_class::default_setting_dir_path();                                                          \
    }                                                                                                          \
    std::optional<std::filesystem::path> temp_dir_path() const                                                 \
    {                                                                                                          \
        return imp_class::temp_dir_path();                                                                     \
    }                                                                                                          \
    std::optional<std::filesystem::path> plugin_dir_path() const                                               \
    {                                                                                                          \
        return imp_class::plugin_dir_path();                                                                   \
    }
