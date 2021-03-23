#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/manager_module.h>
#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/service_setting.h>
#include <kiwi.fast.plugin_utility/setting_visitor.h>
#include <kiwi.fast.plugin_utility/ptree_item.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <map>
#include <string>
#include <any>
#include <filesystem>
#include <optional>
#include <algorithm>
#include <memory>

//为了获取当前用户目录
#if defined(KIWI_FAST_WINDOWS)
#include <windows.h>
#elif defined(KIWI_FAST_LINUX)
#include <stdlib.h>
#endif

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_setting_imp
{
public:
    using setting_index_type = unsigned int;
    using setting_path_type = std::filesystem::path;

    virtual std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_default()
    {
        return m_setting_default;
    }

    virtual std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_user()
    {
        return m_setting_user;
    }

    virtual std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_tmp()
    {
        return m_setting_tmp;
    }

protected:
    service_setting_imp()
        : m_setting_default(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_root())
        , m_setting_user(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_root())
        , m_setting_tmp(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_root())
    {
        //添加基本缺省信息(这些一般是系统启动才确定或者移动程序就会发生改变的信息，系统必需路径)
        add_basic_default_setting();

        ////加载缺省配置
        load_default_setting();

        ////加载用户配置
        load_user_setting();
    }

    virtual ~service_setting_imp()
    {
        //保存用户配置
        save_user_setting();
    }

    /*!
     * \brief add_basic_default_setting
     * 添加基本缺省信息(这些一般是系统启动才确定或者移动程序就会发生改变的信息)
     */
    void add_basic_default_setting()
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value bin_dir_path((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::filesystem::path>()));
        m_bin_dir_path = std::filesystem::current_path();
        bin_dir_path.value<std::filesystem::path>() = m_bin_dir_path;
        m_setting_default->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"bin_dir_path", std::move(bin_dir_path)));

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value root_dir_path((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::filesystem::path>()));
        m_root_dir_path = std::filesystem::current_path().parent_path();
        root_dir_path.value<std::filesystem::path>() = m_root_dir_path;
        m_setting_default->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"root_dir_path", std::move(root_dir_path)));

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value default_setting_dir_path((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::filesystem::path>()));
        m_default_setting_dir_path = std::filesystem::current_path().parent_path() / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"default_setting"));
        default_setting_dir_path.value<std::filesystem::path>() = m_default_setting_dir_path;
        m_setting_default->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"default_setting_dir_path", std::move(default_setting_dir_path)));

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value temp_dir_path((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::filesystem::path>()));
        m_temp_dir_path = std::filesystem::current_path().parent_path() / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"temp"));
        temp_dir_path.value<std::filesystem::path>() = m_temp_dir_path;
        m_setting_default->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"temp_dir_path", std::move(temp_dir_path)));

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value plugin_dir_path((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::filesystem::path>()));
        m_plugin_dir_path = std::filesystem::current_path().parent_path() / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"plugin"));
        plugin_dir_path.value<std::filesystem::path>() = m_plugin_dir_path;
        m_setting_default->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"plugin_dir_path", std::move(plugin_dir_path)));
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

    ///*!
    // * \brief parse_setting_file
    // * 解析配置文件
    // * \param file_info
    // * \param setting
    // */
    void parse_setting_file(std::pair<setting_index_type, setting_path_type> const& file_info, std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> const& setting)
    {
        boost::property_tree::ptree tree;
        boost::property_tree::read_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(file_info.second.native()), tree);

        std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> root = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_root_by_ptree(tree);

        if (root->item_set_value())
        {
            for (auto const& basic_ptree_item : (*root->item_set_value()).get())
            {
                setting->add(basic_ptree_item);
            }
        }
    }

    /*!
     * \brief load_default_setting
     * 加载缺省配置
     */
    void load_default_setting()
    {
        std::deque<std::pair<setting_index_type, setting_path_type>> default_setting_file_info;

        if (!std::filesystem::exists(m_default_setting_dir_path))
        {
            return;
        }

		std::filesystem::directory_iterator end;
		for (std::filesystem::directory_iterator iter(m_default_setting_dir_path); iter != end; ++iter)
		{
			if (std::filesystem::is_regular_file(*iter))
			{
				std::wstring setting_path = iter->path().native();

				boost::property_tree::ptree tree;
				boost::property_tree::read_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(iter->path().native()), tree);

                std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_default = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_root_by_ptree(tree);

				setting_index_type index = 99999;
				auto index_value = m_setting_default->item(u8"index")->value();
				if (index_value)
				{
					index = (*index_value).get().value<unsigned int>();
				}

				default_setting_file_info.push_back(std::make_pair(index, iter->path()));
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

    void load_user_setting()
    {
#if defined(KIWI_FAST_WINDOWS)
        char* home = std::getenv("USERPROFILE");
        if (home)
        {
            m_user_dir_path = std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(home)) 
                / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"kiwi.fast"));
        }
#elif defined(KIWI_FAST_LINUX)
        char* home = std::getenv("HOME");
        if (home)
        {
            m_user_dir_path = std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(home)) 
                / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"kiwi.fast"));
        }
#endif

        if (m_user_dir_path.empty())
        {
            return;
        }

		std::filesystem::create_directories(m_user_dir_path);
		if (std::filesystem::exists(m_user_dir_path))
		{
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value user_dir_path((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::filesystem::path>()));
			user_dir_path.value<std::filesystem::path>() = m_user_dir_path;
			m_setting_default->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"user_dir_path", std::move(user_dir_path)));
		}

        if (!std::filesystem::exists(m_user_dir_path))
        {
            return;
        }

        m_user_file_path = m_user_dir_path / std::filesystem::path(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"user_setting"));

		KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value user_file_path((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::filesystem::path>()));
        user_file_path.value<std::filesystem::path>() = m_user_file_path;
		m_setting_default->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"user_file_path", std::move(user_file_path)));

        if (std::filesystem::exists(m_user_file_path))
        {
            boost::property_tree::ptree tree;
            boost::property_tree::read_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(m_user_file_path.native()), tree);

            m_setting_user = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_root_by_ptree(tree);
        }
    }

    void save_user_setting()
    {
        //保存的过程中可能遇到权限不足的情况，以后补充上

        if (!m_user_file_path.empty())
        {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::ptree_type ptree = m_setting_user->to_root_ptree();

            boost::property_tree::write_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(m_user_file_path.native()), ptree);
        }
    }

private:
    std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> m_setting_default;      //缺省配置
    std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> m_setting_user;         //用户配置
    std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> m_setting_tmp;          //临时配置

    //因为本类初始化过程中不能读取配置（否则无限循环），所以把初始化过程中需要使用的配置暂存在这里
    std::filesystem::path m_bin_dir_path;
    std::filesystem::path m_root_dir_path;
    std::filesystem::path m_default_setting_dir_path;
    std::filesystem::path m_temp_dir_path;
    std::filesystem::path m_plugin_dir_path;
    std::filesystem::path m_user_dir_path;
    std::filesystem::path m_user_file_path;
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE

#define SERVICE_SETTING_ADAPTER_METHOD(imp_class)                                                                                   \
    std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_default()                                 \
    {                                                                                                                               \
        return imp_class::setting_default();                                                                                        \
    }                                                                                                                               \
    std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_user()                                    \
    {                                                                                                                               \
        return imp_class::setting_user();                                                                                           \
    }                                                                                                                               \
    std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> setting_tmp()                                     \
    {                                                                                                                               \
        return imp_class::setting_tmp();                                                                                            \
    }
