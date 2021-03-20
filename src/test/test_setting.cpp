#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>
#include <kiwi.fast.plugin_utility/manager_module.h>
#include <kiwi.fast.utility/src/manager_external_interface_imp.h>

#include <filesystem>

class wmain_instance
{
public:
    class module_manager
    {
    public:
        module_manager()
        {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->set_external_interface_manager(KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER manager_external_interface_imp::instance());
        }

        ~module_manager()
        {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->set_external_interface_manager(nullptr);
            KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER manager_external_interface_imp::destroy();
        }
    };

public:
    wmain_instance(int argc, wchar_t* argv[], wchar_t* envp[])
    {
        //service_exe_run_mode_adapter
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_exe_run_mode_adapter* service_exe_run_mode_adapter_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_exe_run_mode", reinterpret_cast<void**>(&service_exe_run_mode_adapter_));
        if (!service_exe_run_mode_adapter_)
        {
            exit(-1);
        }
        service_exe_run_mode_adapter_->set_batch();

        //service_object_factory_adapter
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_object_factory_adapter* service_object_factory_adapter_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_object_factory", reinterpret_cast<void**>(&service_object_factory_adapter_));

        //service_command_line_adapter
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_command_line_adapter* service_command_line_adapter_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_command_line", reinterpret_cast<void**>(&service_command_line_adapter_));
        if (!service_command_line_adapter_)
        {
            exit(-1);
        }
        service_command_line_adapter_->parse(argc, argv, envp);

        //service_setting_adapter
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_setting_adapter* service_setting_adapter_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_setting", reinterpret_cast<void**>(&service_setting_adapter_));

        //service_log_adapter
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_log_adapter* service_log_adapter_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_log", reinterpret_cast<void**>(&service_log_adapter_));
    }

    ~wmain_instance()
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query_destroy(u8"service_log");
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query_destroy(u8"service_setting");
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query_destroy(u8"service_command_line");
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query_destroy(u8"service_object_factory");
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query_destroy(u8"service_exe_run_mode");
    }

    template<typename F>
    int run(F f)
    {
        return f();
    }

private:
    module_manager m_module_manager;
};

bool init_unit_test()
{
    return true;
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    try
    {
        wmain_instance wmain_instance_(argc, argv, envp);
        return wmain_instance_.run([&]() {
            char** argv_ = new char* [argc];
            for (int i = 0; i < argc; ++i)
            {
                argv_[i] = strdup(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(argv[i]).c_str());
            }

            int result = ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv_);

            for (int i = 0; i < argc; ++i)
            {
                free(argv_[i]);
            }
            delete[] argv_;

            return result;
            });
    }
    catch (KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER exception& ex)
    {
        std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"异常：")
            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(*boost::get_error_info<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER what_info>(ex))
            << '\n';

        std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"文件：")
            << *boost::get_error_info<boost::throw_file>(ex)
            << '\n';

        std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"行号：")
            << *boost::get_error_info<boost::throw_line>(ex)
            << '\n';
    }
}

BOOST_AUTO_TEST_SUITE(s_test_setting)

BOOST_AUTO_TEST_CASE(c_test_setting)
{
    //测试基本缺省信息
    //KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_setting> setting_service;
    //std::optional<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER resource_object_factory<std::filesystem::path>> bin_dir_path = setting_service->setting(u8"bin_dir_path").value<std::filesystem::path>();
    //if (bin_dir_path)
    //{
    //    //std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"bin_dir_path:")
    //    //    << (*bin_dir_path)->string()
    //    //    << '\n';
    //}
    //else
    //{
    //    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"没有 bin_dir_path 配置")
    //        << '\n';
    //}
}

BOOST_AUTO_TEST_SUITE_END()