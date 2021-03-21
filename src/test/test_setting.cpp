#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>
#include <kiwi.fast.plugin_utility/manager_module.h>
#include <kiwi.fast.utility/src/manager_external_interface_imp.h>

#include <kiwi.fast.plugin_utility/data_value.h>

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

        //service_data_value_adapter
        KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER service_data_value_adapter* service_data_value_adapter_ = nullptr;
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query(u8"service_data_value", reinterpret_cast<void**>(&service_data_value_adapter_));

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
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER manager_module::instance()->external_interface_manager()->query_destroy(u8"service_data_value");
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

BOOST_AUTO_TEST_CASE(c_test_service_object_factory)
{
    //测试 service_object_factory 类的函数 create_object 、 copy_object 和 destroy_object

    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;

    //bool
    {
        std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"bool:") << '\n';

        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>();

        std::any object = object_factory_service->create_object(class_name);
        std::cout << "std::any name:" << object.type().name() << '\n';

        bool* value = std::any_cast<bool*>(object);
        *value = true;

        std::any copy_object = object_factory_service->copy_object(class_name, value);
        std::cout << "std::any name:" << copy_object.type().name() << '\n';

        bool* copy_value = std::any_cast<bool*>(copy_object);

        BOOST_CHECK(*copy_value == *value);

        std::any object_any(value);
        object_factory_service->destroy_object(object_any, class_name);

        std::any copy_object_any(copy_value);
        object_factory_service->destroy_object(copy_object_any, class_name);
    }

    //std::deque<bool*>
    {
        std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"std::deque<bool*>:") << '\n';

        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<bool*>>();

        std::any object = object_factory_service->create_object(class_name);
        std::cout << "std::any name:" << object.type().name() << '\n';

        std::deque<bool*>* value = std::any_cast<std::deque<bool*>*>(object);
        bool* value_element = std::any_cast<bool*>(object_factory_service->create_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>()));
        *value_element = true;
        value->push_back(value_element);

        std::any copy_object = object_factory_service->copy_object(class_name, value);
        std::cout << "std::any name:" << copy_object.type().name() << '\n';

        std::deque<bool*>* copy_value = std::any_cast<std::deque<bool*>*>(copy_object);
        bool* copy_value_element = copy_value->at(0);

        BOOST_CHECK(*copy_value_element == *value_element);

        std::any object_any(value);
        object_factory_service->destroy_object(object_any, class_name);

        std::any copy_object_any(copy_value);
        object_factory_service->destroy_object(copy_object_any, class_name);
    }

    //... 其他所有支持类型
}

BOOST_AUTO_TEST_CASE(c_test_data_value)
{
    //data_value 依赖 service_object_factory

    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv;
        BOOST_CHECK(!dv.is_valid());
    }

	{//添加 删除
        bool* b = nullptr;
		{
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<bool>()));  //用括号包住参数，防止被认为是函数声明
			dv.value<bool>() = true;
            b = dv.release<bool>();
		}

        BOOST_CHECK(*b == true);

        {
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv;
            dv.reset(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<bool>(b));

            BOOST_CHECK(dv.value<bool>() == true);
        }
	}

    //bool
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<bool>()));  //用括号包住参数，防止被认为是函数声明
        BOOST_CHECK(dv.is_valid());

        dv.value<bool>() = true;

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv2(dv);

        *dv.pointer<bool>() = false;

        BOOST_CHECK(dv.value<bool>() == false);
        BOOST_CHECK(dv2.value<bool>() == true);

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv3;
        dv3 = dv;

        *dv.pointer<bool>() = true;

        BOOST_CHECK(dv.value<bool>() == true);
        BOOST_CHECK(dv3.value<bool>() == false);

        BOOST_CHECK(dv == dv2);
        BOOST_CHECK(dv != dv3);

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv4(std::move(dv));

        BOOST_CHECK(!dv.is_valid());
        BOOST_CHECK(dv4.value<bool>() == true);
    }

    //std::deque<bool*>
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_deque_value<bool>()));  //用括号包住参数，防止被认为是函数声明
        BOOST_CHECK(dv.is_valid());

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_element((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<bool>()));  //用括号包住参数，防止被认为是函数声明
        dv_element.value<bool>() = true;

        dv.value<std::deque<bool*>>().push_back(dv_element.release<bool>());

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv2(dv);

        *dv.pointer<std::deque<bool*>>()->at(0) = false;

        BOOST_CHECK(*dv.value<std::deque<bool*>>().at(0) == false);
        BOOST_CHECK(*dv2.value<std::deque<bool*>>().at(0) == true);

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv3;
        dv3 = dv;

        *dv.pointer<std::deque<bool*>>()->at(0) = true;

        BOOST_CHECK(*dv.value<std::deque<bool*>>().at(0) == true);
        BOOST_CHECK(*dv2.value<std::deque<bool*>>().at(0) == true);
        BOOST_CHECK(*dv3.value<std::deque<bool*>>().at(0) == false);

        BOOST_CHECK(dv == dv2);
        BOOST_CHECK(dv != dv3);

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv4(std::move(dv));

        BOOST_CHECK(!dv.is_valid());
        BOOST_CHECK(*dv4.value<std::deque<bool*>>().at(0) == true);
    }

}

BOOST_AUTO_TEST_CASE(c_test_service_data_value)
{
    //service_data_value 辅助 data_value，主要做一些字符串转类型的初始化工作
}

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