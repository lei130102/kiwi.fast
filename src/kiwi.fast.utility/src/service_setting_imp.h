#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/manager_module.h>
#include <kiwi.fast.plugin_utility/ptree_root.h>
#include <kiwi.fast.plugin_utility/resource_object_factory.h>
#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/service_setting.h>
#include <kiwi.fast.plugin_utility/setting_visitor.h>

#include <kiwi.fast.utility/src/service_object_factory_adapter.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <map>
#include <string>
#include <any>
#include <filesystem>
#include <optional>
#include <algorithm>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

template<typename Setting>
class service_visitor;

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_setting_imp
{
    friend class KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_visitor<service_setting_imp>;

public:
    using setting_index_type = unsigned int;
    using setting_path_type = std::filesystem::path;

    virtual KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root& setting_default()
    {
        return m_setting_default;
    }

    virtual KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root& setting_user()
    {
        return m_setting_user;
    }

    virtual KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root& setting_tmp()
    {
        return m_setting_tmp;
    }

    /*!
     * 与service_setting类中的同名函数功能一致
     * 因为service_setting_imp类初始化先于service_setting类创建，所以service_setting_imp类初始化过程中只能使用这个函数
     */
    template<typename CharType>
    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER setting_visitor<service_setting_imp> setting(CharType const* name)
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER setting_visitor<service_setting_imp> visitor(this);
        visitor.add_name(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name));
        return visitor;
    }

    /*!
     * 与service_setting类中的同名函数功能一致
     * 因为service_setting_imp类初始化先于service_setting类创建，所以service_setting_imp类初始化过程中只能使用这个函数
     */
    template<typename CharType>
    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER setting_visitor<service_setting_imp> tmp_setting(CharType const* name)
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER setting_visitor<service_setting_imp> visitor(this);
        visitor.set_tmp(true);
        visitor.add_name(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(name));
        return visitor;
    }

    //常用的列在这里
    virtual std::optional<std::filesystem::path*> bin_dir_path()
    {
        //return setting(u8"bin_dir_path").value<std::filesystem::path>();
        return {};
    }

    virtual std::optional<std::filesystem::path*> root_dir_path()
    {
        //return setting(u8"root_dir_path").value<std::filesystem::path>();
        return {};
    }

    virtual std::optional<std::filesystem::path*> default_setting_dir_path()
    {
        //return setting(u8"default_setting_dir_path").value<std::filesystem::path>();
        return {};
    }

    virtual std::optional<std::filesystem::path*> temp_dir_path()
    {
        //return setting(u8"temp_dir_path").value<std::filesystem::path>();
        return {};
    }

    virtual std::optional<std::filesystem::path*> plugin_dir_path()
    {
        //return setting(u8"plugin_dir_path").value<std::filesystem::path>();
        return {};
    }

protected:
    service_setting_imp()
    {
        //添加基本缺省信息(这些一般是系统启动才确定或者移动程序就会发生改变的信息，系统必需路径)
        add_basic_default_setting();

        //加载缺省配置
        //load_default_setting();

        //加载用户配置

        //导入临时配置
    }

    virtual ~service_setting_imp()
    {
        //卸载缺省配置
        //unload_default_setting();

        //卸载用户配置
    }

    /*!
     * \brief add_basic_default_setting
     * 添加基本缺省信息(这些一般是系统启动才确定或者移动程序就会发生改变的信息)
     */
    void add_basic_default_setting()
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER resource_object_factory<std::filesystem::path> bin_dir_path;
        *bin_dir_path = std::filesystem::current_path();
        m_setting_default.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_item(u8"bin_dir_path", bin_dir_path));

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER resource_object_factory<std::filesystem::path> root_dir_path;
        *root_dir_path = bin_dir_path->parent_path();
        m_setting_default.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_item(u8"root_dir_path", root_dir_path));

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER resource_object_factory<std::filesystem::path> default_setting_dir_path;
        *default_setting_dir_path = *root_dir_path / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"default_setting"));
        m_setting_default.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_item(u8"default_setting_dir_path", default_setting_dir_path));

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER resource_object_factory<std::filesystem::path> temp_dir_path;
        *temp_dir_path = *root_dir_path / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"temp"));
        m_setting_default.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_item(u8"temp_dir_path", temp_dir_path));

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER resource_object_factory<std::filesystem::path> plugin_dir_path;
        *plugin_dir_path = *root_dir_path / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"plugin"));
        m_setting_default.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_item(u8"plugin_dir_path", plugin_dir_path));
    }

    struct setting_index_less
    {
        bool operator()(std::pair<setting_index_type, setting_path_type> const& lhs, std::pair<setting_index_type, setting_path_type> const& rhs)
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
    void parse_setting_file(std::pair<setting_index_type, setting_path_type> const& file_info, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root& setting)
    {
        boost::property_tree::ptree tree;
        boost::property_tree::read_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(file_info.second.native()), tree);
        setting = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::from_ptree(tree);
    }

    /*!
     * \brief load_default_setting
     * 加载缺省配置
     */
    void load_default_setting()
    {
        std::deque<std::pair<setting_index_type, setting_path_type>> default_setting_file_info;

        auto default_setting_dir_path_ = default_setting_dir_path();
        if(default_setting_dir_path_)
        {
            std::filesystem::directory_iterator end;
            for(std::filesystem::directory_iterator iter(**default_setting_dir_path_); iter != end; ++iter)
            {
                if(std::filesystem::is_regular_file(*iter))
                {
                    //日志输出(此时日志类没有初始化)
                    //std::wstring setting_path = iter->path().native();

                    //boost::property_tree::ptree tree;
                    //boost::property_tree::read_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(iter->path().native()), tree);

                    //KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root tmp = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::from_ptree(tree);

                    //KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_visitor tmp_visitor(&tmp);
                    //KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER resource_object_factory<setting_index_type> default_index;
                    //*default_index = 999;



                    //setting_index_type* setting_index = tmp_visitor(u8"kiwi_fast_index").value<setting_index_type>(*default_index);

                    //default_setting_file_info.push_back(std::make_pair(*setting_index, iter->path()));
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

    /*!
     * \brief unload_default_setting
     * 卸载缺省配置
     */
    void unload_default_setting()
    {
//        service_object_factory_imp* service_object_factory_imp_ = nullptr;
//        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(
//                    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory>()
//                    , reinterpret_cast<void**>(&service_object_factory_imp_));
//        if(!service_object_factory_imp_)
//        {
//            return;
//        }

//        m_setting_default.clear();
    }

private:
    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root m_setting_default;      //缺省配置
    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root m_setting_user;         //用户配置
    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root m_setting_tmp;          //临时配置
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE

#define SERVICE_SETTING_ADAPTER_METHOD(imp_class)                                                              \
    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root& setting_default()                                 \
    {                                                                                                          \
        return imp_class::setting_default();                                                                   \
    }                                                                                                          \
    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root& setting_user()                                    \
    {                                                                                                          \
        return imp_class::setting_user();                                                                      \
    }                                                                                                          \
    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root& setting_tmp()                                     \
    {                                                                                                          \
        return imp_class::setting_tmp();                                                                       \
    }                                                                                                          \
    std::optional<std::filesystem::path*> bin_dir_path()                                                       \
    {                                                                                                          \
        return imp_class::bin_dir_path();                                                                      \
    }                                                                                                          \
    std::optional<std::filesystem::path*> root_dir_path()                                                      \
    {                                                                                                          \
        return imp_class::root_dir_path();                                                                     \
    }                                                                                                          \
    std::optional<std::filesystem::path*> default_setting_dir_path()                                           \
    {                                                                                                          \
        return imp_class::default_setting_dir_path();                                                          \
    }                                                                                                          \
    std::optional<std::filesystem::path*> temp_dir_path()                                                      \
    {                                                                                                          \
        return imp_class::temp_dir_path();                                                                     \
    }                                                                                                          \
    std::optional<std::filesystem::path*> plugin_dir_path()                                                    \
    {                                                                                                          \
        return imp_class::plugin_dir_path();                                                                   \
    }
