#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>
#include <kiwi.fast.plugin_utility/manager_module.h>
#include <kiwi.fast.plugin_utility/ptree_item.h>
#include <kiwi.fast.plugin_utility/ptree_visitor.h>
#include <kiwi.fast.utility/src/manager_external_interface_imp.h>
#include <kiwi.fast.utility/src/register_type.h>

#include <kiwi.fast.plugin_utility/data_value.h>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <memory>
#include <filesystem>
#include <string>
#include <deque>

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

    //std::deque<char*>的无元素、一个元素、两个元素时与字符串转换

    //没有元素的值与字符串转换
    {
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<char*>>();
        std::any object = object_factory_service->create_object(class_name);

        auto str = object_factory_service->object_to_string(object, class_name);

        BOOST_CHECK(str && (*str).empty());

        object_factory_service->destroy_object(object, class_name);

        //

        std::any empty_deque_from_string;
        object_factory_service->object_from_string(std::u8string(), class_name, empty_deque_from_string);

        BOOST_CHECK(std::any_cast<std::deque<char*>*>(empty_deque_from_string)->size() == 0);

        object_factory_service->destroy_object(empty_deque_from_string, class_name);
    }
    //有一个元素的值与字符串转换
    {
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<char*>>();
        std::any object = object_factory_service->create_object(class_name);
        char* element = std::any_cast<char*>(object_factory_service->create_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<char>()));
        *element = 97;
        //或者
        //*element = 'a';
        std::any_cast<std::deque<char*>*>(object)->push_back(element);

        auto str = object_factory_service->object_to_string(object, class_name);

        BOOST_CHECK(str && (*str) == u8"97"); //注意

        object_factory_service->destroy_object(object, class_name);

        //

        std::any one_element_from_string;
        object_factory_service->object_from_string(*str, class_name, one_element_from_string);

        std::deque<char*>* deque_from_string = std::any_cast<std::deque<char*>*>(one_element_from_string);

        BOOST_CHECK(deque_from_string->size() == 1);
        BOOST_CHECK(*deque_from_string->front() == 97);
        //或者
        //BOOST_CHECK(*deque_from_string->front() == 'a');

        object_factory_service->destroy_object(one_element_from_string, class_name);
    }
    //有两个元素的值与字符串转换
    {
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<char*>>();
        std::any object = object_factory_service->create_object(class_name);
        char* element = std::any_cast<char*>(object_factory_service->create_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<char>()));
        *element = 97;
        //或者
        //*element = 'a';
        std::any_cast<std::deque<char*>*>(object)->push_back(element);
        char* element2 = std::any_cast<char*>(object_factory_service->create_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<char>()));
        *element2 = 98;
        //或者
        //*element = 'b';
        std::any_cast<std::deque<char*>*>(object)->push_back(element2);

        auto str = object_factory_service->object_to_string(object, class_name);

        std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(*str) << '\n';

        BOOST_CHECK(str && (*str) == u8"97‖98"); //注意

        object_factory_service->destroy_object(object, class_name);

        //

        std::any two_element_from_string;
        object_factory_service->object_from_string(*str, class_name, two_element_from_string);

        std::deque<char*>* deque_from_string = std::any_cast<std::deque<char*>*>(two_element_from_string);

        BOOST_CHECK(deque_from_string->size() == 2);
        BOOST_CHECK(*deque_from_string->at(0) == 97);
        //或者
        //BOOST_CHECK(*deque_from_string->front() == 'a');
        BOOST_CHECK(*deque_from_string->at(1) == 98);
        //或者
        //BOOST_CHECK(*deque_from_string->front() == 'b');

        object_factory_service->destroy_object(two_element_from_string, class_name);
    }


    //bool
    {
        std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"bool:") << '\n';

        //create_object

        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>();

        std::any object = object_factory_service->create_object(class_name);
        std::cout << "std::any name:" << object.type().name() << '\n';

        bool* value = std::any_cast<bool*>(object);
        *value = true;

        //copy_object

        std::any copy_object = object_factory_service->copy_object(class_name, value);
        std::cout << "std::any name:" << copy_object.type().name() << '\n';

        bool* copy_value = std::any_cast<bool*>(copy_object);

        BOOST_CHECK(*copy_value == *value);

        //object_to_string
        auto str = object_factory_service->object_to_string(copy_object, class_name);
        if (str)
        {
            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"object_to_string:")
                << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(*str) << '\n';

            //object_from_string

            std::any any_from_string;
            object_factory_service->object_from_string(*str, class_name, any_from_string);//object_from_string要求any_from_string参数为初始std::any

            BOOST_CHECK(*std::any_cast<bool*>(any_from_string) == *copy_value);

            //destroy_object

            object_factory_service->destroy_object(any_from_string, class_name);
        }

        std::any object_any(value);
        object_factory_service->destroy_object(object_any, class_name);

        std::any copy_object_any(copy_value);
        object_factory_service->destroy_object(copy_object_any, class_name);
    }

    //std::deque<bool*>
    {
        std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"std::deque<bool*>:") << '\n';

        //create_object

        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<bool*>>();

        std::any object = object_factory_service->create_object(class_name);
        std::cout << "std::any name:" << object.type().name() << '\n';

        std::deque<bool*>* value = std::any_cast<std::deque<bool*>*>(object);
        bool* value_element = std::any_cast<bool*>(object_factory_service->create_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>()));
        *value_element = true;
        value->push_back(value_element);

        //copy_object

        std::any copy_object = object_factory_service->copy_object(class_name, value);
        std::cout << "std::any name:" << copy_object.type().name() << '\n';

        std::deque<bool*>* copy_value = std::any_cast<std::deque<bool*>*>(copy_object);
        bool* copy_value_element = copy_value->at(0);

        BOOST_CHECK(*copy_value_element == *value_element);

        //object_to_string
        auto str = object_factory_service->object_to_string(copy_object, class_name);
        if (str)
        {
            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"object_to_string:")
                << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(*str) << '\n';

            //object_from_string

            std::any any_from_string;
            object_factory_service->object_from_string(*str, class_name, any_from_string);//object_from_string要求any_from_string参数为初始std::any

            std::deque<bool*>* from_string_value = std::any_cast<std::deque<bool*>*>(any_from_string);
            bool* from_string_value_element = from_string_value->at(0);
            BOOST_CHECK(*from_string_value_element == *value_element);

            //destroy_object

            object_factory_service->destroy_object(any_from_string, class_name);
        }

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
        BOOST_CHECK(dv2.value<bool>() == true);
        BOOST_CHECK(dv3.value<bool>() == false);

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

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv4(std::move(dv));

        BOOST_CHECK(!dv.is_valid());
        BOOST_CHECK(*dv4.value<std::deque<bool*>>().at(0) == true);
    }

    //std::u8string
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::u8string>()));  //用括号包住参数，防止被认为是函数声明
        BOOST_CHECK(dv.is_valid());

        dv.value<std::u8string>() = u8"张三";

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv2(dv);

        *dv.pointer<std::u8string>() = u8"李四";

        BOOST_CHECK(dv.value<std::u8string>() == u8"李四");
        BOOST_CHECK(dv2.value<std::u8string>() == u8"张三");

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv3;
        dv3 = dv;

        *dv.pointer<std::u8string>() = u8"钱一";

        BOOST_CHECK(dv.value<std::u8string>() == u8"钱一");
        BOOST_CHECK(dv2.value<std::u8string>() == u8"张三");
        BOOST_CHECK(dv3.value<std::u8string>() == u8"李四");

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv4(std::move(dv));

        BOOST_CHECK(!dv.is_valid());
        BOOST_CHECK(dv4.value<std::u8string>() == u8"钱一");
    }
}

BOOST_AUTO_TEST_CASE(c_test_service_data_value)
{
    //service_data_value 辅助 data_value，主要做一些字符串转类型的初始化工作

    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_data_value> data_value_service;

    //std::deque<char8_t*>的无元素、一个元素、两个元素时与字符串转换

    //没有元素的值与字符串转换
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv = data_value_service->create_data_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<char8_t*>>());

        auto str = data_value_service->data_value_to_string(dv);
        BOOST_CHECK(str && (*str).empty());

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_from_string;
        data_value_service->data_value_from_string(std::u8string(), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<char8_t*>>(), dv_from_string);

        BOOST_CHECK(dv_from_string.value<std::deque<char8_t*>>().size() == 0);
    }
    //有一个元素的值与字符串转换
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv = data_value_service->create_data_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<char8_t*>>());

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_element((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<char8_t>()));
        dv_element.value<char8_t>() = u8'a';
        dv.value<std::deque<char8_t*>>().push_back(dv_element.release<char8_t>());

        auto str = data_value_service->data_value_to_string(dv);

        BOOST_CHECK(str && (*str) == u8"97");//注意

        //

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_from_string;
        data_value_service->data_value_from_string(*str, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<char8_t*>>(), dv_from_string);

        BOOST_CHECK(dv.value<std::deque<char8_t*>>().size() == 1);
        BOOST_CHECK(*dv.value<std::deque<char8_t*>>().front() == 97);
    }
    //有两个元素的值与字符串转换
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv = data_value_service->create_data_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<char8_t*>>());

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_element((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<char8_t>()));
        dv_element.value<char8_t>() = u8'a';
        dv.value<std::deque<char8_t*>>().push_back(dv_element.release<char8_t>());
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_element2((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<char8_t>()));
        dv_element2.value<char8_t>() = u8'b';
        dv.value<std::deque<char8_t*>>().push_back(dv_element2.release<char8_t>());

        auto str = data_value_service->data_value_to_string(dv);

        BOOST_CHECK(str && (*str) == u8"97‖98");//注意

        //

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_from_string;
        data_value_service->data_value_from_string(*str, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<char8_t*>>(), dv_from_string);

        BOOST_CHECK(dv.value<std::deque<char8_t*>>().size() == 2);
        BOOST_CHECK(*dv.value<std::deque<char8_t*>>().at(0) == 97);
        BOOST_CHECK(*dv.value<std::deque<char8_t*>>().at(1) == 98);
    }


    //bool
    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv = data_value_service->create_data_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>());
        dv.value<bool>() = true;

        BOOST_CHECK(dv.value<bool>() == true);

        auto str = data_value_service->data_value_to_string(dv);
        if (str)
        {
            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"data_value_to_string:") 
                << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(*str) << '\n';

            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_from_string;
            data_value_service->data_value_from_string(*str, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>(), dv_from_string);

            BOOST_CHECK(dv.value<bool>() == true);
        }
    }

    {
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv = data_value_service->create_data_value(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<bool*>>());

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_element((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<bool>()));  //用括号包住参数，防止被认为是函数声明
        dv_element.value<bool>() = true;

        dv.value<std::deque<bool*>>().push_back(dv_element.release<bool>());

        BOOST_CHECK(*dv.value<std::deque<bool*>>().front() == true);

        auto str = data_value_service->data_value_to_string(dv);
        if (str)
        {
            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"data_value_to_string:")
                << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(*str) << '\n';

            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_from_string;
            data_value_service->data_value_from_string(*str, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<bool*>>(), dv_from_string);

            BOOST_CHECK(*dv.value<std::deque<bool*>>().front() == true);
        }
    }

    //...
}

BOOST_AUTO_TEST_CASE(c_test_data_value_with_service_data_value_help)
{
    //将 service_data_value 提供的功能再集成到 data_value，只是为了只使用data_value会比较方便

    //下面的方法实际调用的是 service_data_value 的方法

    {
        //data_value::create

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value::create(
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>());
        dv.value<bool>() = true;

        //data_value::to_string

        auto str = dv.to_string();

        BOOST_CHECK(str && *str == u8"1");

        //data_value::from_string

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_from_string = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value::from_string(*str
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>());
        BOOST_CHECK(dv_from_string.is_valid());
        BOOST_CHECK(dv_from_string.value<bool>() == true);
    }
}

BOOST_AUTO_TEST_CASE(c_test_ptree_item)
{
    {
        //创建ptree_root

		auto root = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_root();

		KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value name((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::u8string>()));
		name.value<std::u8string>() = u8"张三";
        root->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"name", std::move(name)));

        auto info_item = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item_set(u8"info");

		KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value sex((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::u8string>()));
		sex.value<std::u8string>() = u8"男";
        info_item->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"sex", std::move(sex)));

		KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value age((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<int>()));
		age.value<int>() = 23;
        info_item->add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"age", std::move(age)));

        root->add(info_item);

        //转为ptree类型

		KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::ptree_type ptree = root->to_root_ptree();

        //转xml文件
        boost::property_tree::write_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\test.xml)"), ptree);

/*文件内容为：
<?xml version="1.0" encoding="utf-8"?>
<kiwi_fast>
	<item>
		<name>name</name>
		<type>std::u8string</type>
		<value>张三</value>
	</item>
	<item>
		<name>info</name>
		<type>ptree_item_set</type>
		<value>
			<item>
				<name>sex</name>
				<type>std::u8string</type>
				<value>男</value>
			</item>
			<item>
				<name>age</name>
				<type>int</type>
				<value>23</value>
			</item>
		</value>
	</item>
</kiwi_fast>
*/
        //转json文件
        boost::property_tree::write_json(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\test.json)"), ptree);

/*文件内容为：
{
    "kiwi_fast": {
        "item": {
            "name": "name",
            "type": "std::u8string",
            "value": "张三"
        },
        "item": {
            "name": "info",
            "type": "ptree_item_set",
            "value": {
                "item": {
                    "name": "sex",
                    "type": "std::u8string",
                    "value": "男"
                },
                "item": {
                    "name": "age",
                    "type": "int",
                    "value": "23"
                }
            }
        }
    }
}
*/

        //转ini文件
        //boost::property_tree::write_ini(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\test.ini)"), ptree);
        
        //抛出异常，并且文件内容为空，应该是因为他不是有效的ini格式


        ////从ptree转为ptree_root
        auto root_from_ptree = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_root_by_ptree(ptree);

        //测试转换是否成功
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::ptree_type ptree_from_root = root_from_ptree->to_root_ptree();
        boost::property_tree::write_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\test_from_root.xml)"), ptree_from_root);


        //使用 ptree_visitor 访问 root_from_ptree

        //读写ptree_item中的name
        std::optional<std::reference_wrapper<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_item::name_type>> age_from_ptree = root_from_ptree->item(u8"info")->item(u8"age")->name();
        if (age_from_ptree)
        {
            //读取
            std::u8string age_str = (*age_from_ptree).get();

            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(age_str) << '\n';
            BOOST_CHECK(age_str == u8"age");

            //修改
            (*age_from_ptree).get() = u8"modified_age";

            //读取
            age_str = (*age_from_ptree).get();

            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(age_str) << '\n';
            BOOST_CHECK(age_str == u8"modified_age");
        }
        
        //读写ptree_item中的data_value
        //ptree_item::value_type类型实际就是data_value类型
        std::optional<std::reference_wrapper<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_item::value_type>> name_from_ptree = root_from_ptree->item(u8"name")->value();
        if (name_from_ptree)
        {
            //读取
            std::u8string name_str = (*name_from_ptree).get().value<std::u8string>();

            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name_str) << '\n';
            BOOST_CHECK(name_str == u8"张三");

            //修改data_value的值
            (*name_from_ptree).get().value<std::u8string>() = u8"李四";

            //读取
            name_str = (*name_from_ptree).get().value<std::u8string>();

            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(name_str) << '\n';
            BOOST_CHECK(name_str == u8"李四");
        }

        std::optional<std::reference_wrapper<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value>> sex_from_ptree = root_from_ptree->item(u8"info")->item(u8"sex")->value();
        if (sex_from_ptree)
        {
            //读取
            std::u8string sex_str = (*sex_from_ptree).get().value<std::u8string>();

            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(sex_str) << '\n';
            BOOST_CHECK(sex_str == u8"男");

            //修改整个data_value，即值和值类型
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_int((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<int>()));
            dv_int.value<int>() = 2;
            (*sex_from_ptree).get() = dv_int;

            //读取
            int sex_int = (*sex_from_ptree).get().value<int>();

            std::cout << sex_int << '\n';
            BOOST_CHECK(sex_int == 2);
        }

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::ptree_type ptree_from_root_modified = root_from_ptree->to_root_ptree();
        boost::property_tree::write_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\test_from_root_modified.xml)"), ptree_from_root_modified);

        //增加ptree_item
        {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<std::u8string>()));
            dv.value<std::u8string>() = u8"门头沟";
            std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> new_item = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"address", std::move(dv));

            root_from_ptree->add(new_item);
        }

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::ptree_type ptree_from_root_add_item = root_from_ptree->to_root_ptree();
        boost::property_tree::write_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\test_from_root_add_item.xml)"), ptree_from_root_add_item);

        //减少ptree_item
        {
            root_from_ptree->remove(u8"address");
        }

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::ptree_type ptree_from_root_remove_item = root_from_ptree->to_root_ptree();
        boost::property_tree::write_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\test_from_root_remove_item.xml)"), ptree_from_root_remove_item);
        
        //增加ptree_item_set
        {
            std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> new_item_set = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item_set(u8"rgb");

            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_r((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<unsigned short>()));
            dv_r.value<unsigned short>() = 235;
            std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> r_item = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"r", std::move(dv_r));

            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_g((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<unsigned short>()));
            dv_g.value<unsigned short>() = 200;
            std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> g_item = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"g", std::move(dv_g));

            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_value dv_b((KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER data_object_value<unsigned short>()));
            dv_b.value<unsigned short>() = 100;
            std::shared_ptr<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER base_ptree_item> b_item = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER create_ptree_item(u8"b", std::move(dv_b));

            new_item_set->add(r_item);
            new_item_set->add(g_item);
            new_item_set->add(b_item);

            root_from_ptree->add(new_item_set);
        }

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::ptree_type ptree_from_root_add_item_set = root_from_ptree->to_root_ptree();
        boost::property_tree::write_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\test_from_root_add_item_set.xml)"), ptree_from_root_add_item_set);

        //减少ptree_item_set
        {
            root_from_ptree->remove(u8"rgb");
        }

        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER ptree_root::ptree_type ptree_from_root_remove_item_set = root_from_ptree->to_root_ptree();
        boost::property_tree::write_xml(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8R"(D:\test_from_root_remove_item_set.xml)"), ptree_from_root_remove_item_set);
	}
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