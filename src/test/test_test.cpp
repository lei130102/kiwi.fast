#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/manager_module.h>
#include <kiwi.fast.utility/src/manager_external_interface_imp.h>

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
        //service_exe_run_mode_imp
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_exe_run_mode_imp* service_exe_run_mode_imp_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_exe_run_mode", reinterpret_cast<void**>(&service_exe_run_mode_imp_));
        if(!service_exe_run_mode_imp_)
        {
            exit(-1);
        }
        service_exe_run_mode_imp_->set_batch();

        //service_object_factory_imp
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_object_factory_imp* service_object_factory_imp_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_object_factory", reinterpret_cast<void**>(&service_object_factory_imp_));

        //service_command_line_imp
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_command_line_imp* service_command_line_imp_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_command_line", reinterpret_cast<void**>(&service_command_line_imp_));

        //service_setting_imp
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_setting_imp* service_setting_imp_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_setting", reinterpret_cast<void**>(&service_setting_imp_));

        //service_log_imp
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_log_imp* service_log_imp_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_log", reinterpret_cast<void**>(&service_log_imp_));
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
    wmain_instance wmain_instance_(argc, argv, envp);
    return wmain_instance_.run([&](){
        char** argv_ = new char*[argc];
        for(int i = 0; i < argc; ++i)
        {
            argv_[i] = strdup(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(argv[i]).c_str());
        }

        int result = ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv_);

        for(int i = 0; i < argc; ++i)
        {
            free(argv_[i]);
        }
        delete[] argv_;

        return result;
    });

    //特别注意！MSYS64环境要求main函数正常返回必须用"return 0"，不能省略他否则崩溃
}

BOOST_AUTO_TEST_SUITE(s_test)

BOOST_AUTO_TEST_CASE(c_test)
{
    BOOST_CHECK(1==1);
}

BOOST_AUTO_TEST_SUITE_END()
