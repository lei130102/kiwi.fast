#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>
#include <kiwi.fast.plugin_utility/manager_module.h>
#include <kiwi.fast.utility/src/manager_external_interface_imp.h>


//测试命令行参数解析功能

//目前不知道怎么和boost.test一起使用，因为boost.test也有命令行参数解析功能

//launch.vs.json是vs下cmake工程的调试配置文件
//举例：
/*
{
"version": "0.2.1",
"defaults" : {},
"configurations" : [
{
    "type": "default",
        "project" : "CMakeLists.txt",
        "projectTarget" : "ncnnoptimize.exe (tools\\ncnnoptimize.exe)",
        "name" : "ncnnoptimize.exe (tools\\ncnnoptimize.exe)",
        "args" : [  // 参数
            "models\\squeezenet_v1.1.param",
                "models\\squeezenet_v1.1.bin",
                "models\\squeezenet_v1.1-nobn-fp32.param",
                "models\\squeezenet_v1.1-nobn-fp32.bin",
                "0"
        ] ,
        "currentDir": "C:\\Users\\mengf\\code\\ncnn"  // 设置工作目录
}
]
}
*/


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
        service_command_line_adapter_->add_option(u8"custom_option_name", u8"自定义选项的描述");
        service_command_line_adapter_->add_int_option(u8"no_default_value_no_implicit_value", u8"选项没有设定 default_value 和 implicit_value"
            );
        service_command_line_adapter_->add_int_option(u8"has_default_value_no_implicit_value", u8"选项设定 default_value ，但没有设定 implicit_value"
            , 10
            );
        service_command_line_adapter_->add_int_option(u8"has_default_value_has_implicit_value", u8"选项设定 default_value 和 implicit_value"
            , 10
            , 20
            );
        service_command_line_adapter_->add_wstring_option(u8"custom_wstring_option_name", u8"自定义std::wstring类型选项的描述"
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"default_value 选项值可以包含中文，但选项名最好是英文")
            , KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"implicit_value 选项值可以包含中文，但选项名最好是英文")
            );
        service_command_line_adapter_->add_vector_wstring_option(u8"custom_vector_wstring_option_name", u8"自定义std::vector<std::wstring>类型选项的描述"
            , std::vector<std::wstring>{KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"a"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"b"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"c")}
            , std::vector<std::wstring>{KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"d"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"e"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"f")}
        );
        service_command_line_adapter_->add_short_option(u8"custom_short_option_name", u8"自定义short类型选项的描述"
            , 1
            , 2);
        service_command_line_adapter_->add_vector_short_option(u8"custom_vector_short_option_name", u8"自定义std::vector<short>类型选项的描述"
            , std::vector<short>{1, 2, 3}
            , std::vector<short>{4, 5, 6}
        );
        service_command_line_adapter_->add_unsigned_short_option(u8"custom_unsigned_short_option_name", u8"自定义unsigned short类型选项的描述"
            , 1
            , 2);
        service_command_line_adapter_->add_vector_unsigned_short_option(u8"custom_vector_unsigned_short_option_name", u8"自定义std::vector<unsigned short>类型选项的描述"
            , std::vector<unsigned short>{1, 2, 3}
            , std::vector<unsigned short>{4, 5, 6}
        );
        service_command_line_adapter_->add_int_option(u8"custom_int_option_name", u8"自定义int类型选项的描述"
            , 10
            , 20);
        service_command_line_adapter_->add_vector_int_option(u8"custom_vector_int_option_name", u8"自定义std::vector<int>类型选项的描述"
            , std::vector<int>{1, 2, 3}
            , std::vector<int>{4, 5, 6}
        );
        service_command_line_adapter_->add_unsigned_int_option(u8"custom_unsigned_int_option_name", u8"自定义unsigned int类型选项的描述"
            , 100
            , 200);
        service_command_line_adapter_->add_vector_unsigned_int_option(u8"custom_vector_unsigned_int_option_name", u8"自定义std::vector<unsigned int>类型选项的描述"
            , std::vector<unsigned int>{1, 2, 3}
            , std::vector<unsigned int>{4, 5, 6}
        );
        service_command_line_adapter_->add_long_long_option(u8"custom_long_long_option_name", u8"自定义long long类型选项的描述"
            , 200
            , 300);
        service_command_line_adapter_->add_vector_long_long_option(u8"custom_vector_long_long_option_name", u8"自定义std::vector<long long>类型选项的描述"
            , std::vector<long long>{1, 2, 3}
            , std::vector<long long>{4, 5, 6}
        );
        service_command_line_adapter_->add_unsigned_long_long_option(u8"custom_unsigned_long_long_option_name", u8"自定义unsigned long long类型选项的描述"
            , 400
            , 500);
        service_command_line_adapter_->add_vector_unsigned_long_long_option(u8"custom_vector_unsigned_long_long_option_name", u8"自定义std::vector<unsigned long long>类型选项的描述"
            , std::vector<unsigned long long>{1, 2, 3}
            , std::vector<unsigned long long>{4, 5, 6}
        );
        service_command_line_adapter_->add_float_option(u8"custom_float_option_name", u8"自定义float类型选项的描述"
            , 4.1f
            , 5.2f);
        service_command_line_adapter_->add_vector_float_option(u8"custom_vector_float_option_name", u8"自定义std::vector<float>类型选项的描述"
            , std::vector<float>{1, 2, 3}
            , std::vector<float>{4, 5, 6}
        );
        service_command_line_adapter_->add_double_option(u8"custom_double_option_name", u8"自定义double类型选项的描述"
            , 6.3
            , 7.4);
        service_command_line_adapter_->add_vector_double_option(u8"custom_vector_double_option_name", u8"自定义std::vector<double>类型选项的描述"
            , std::vector<double>{1, 2, 3}
            , std::vector<double>{4, 5, 6}
        );
        service_command_line_adapter_->add_bool_switch_option(u8"custom_bool_switch_option_name", u8"自定义bool_switch风格选项的描述");
        service_command_line_adapter_->add_bool_switch_option(u8"custom_true_bool_switch_option_name", u8"自定义bool_switch风格默认值为true的选项的描述"
            , true);
        service_command_line_adapter_->parse(argc, argv, envp);
    }

    ~wmain_instance()
    {
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
    //KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"解析命令行参数失败。可能：命令行中没有指定没有implicit_value的选项的选项值");
    try
    {

        wmain_instance wmain_instance_(argc, argv, envp);
        return wmain_instance_.run([&]() {
            char** argv_ = new char* [argc];
            for (int i = 0; i < argc; ++i)
            {
                argv_[i] = strdup(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(argv[i]).c_str());
            }




            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_command_line> command_line_service;

            //1.没有设置且命令行中也没有的选项
            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"no_option_no_command_line 选项个数：") << command_line_service->variable_count(u8"no_option_no_command_line") << "\n";
            if (command_line_service->variable_count(u8"no_option_no_command_line"))
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"不会进入到这里") << "\n";
            }
            else
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"没有设置 no_option_no_command_line 选项，命令行也中没有 no_option_no_command_line 选项") << "\n";
            }


            //2.没有设置但命令行中有的选项
            //无法解析这样的选项并且不会抛出异常，相当于无视


            //3.已设置但命令行中没有的选项

            //3.1选项没有设定 default_value 和 implicit_value
            //测试命令行中：
            //a.没有 --no_default_value_no_implicit_value          结果：(显而易见)
            //b.--no_default_value_no_implicit_value               结果：抛出异常
            //c.--no_default_value_no_implicit_value 150           结果：(显而易见)
			{
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"no_default_value_no_implicit_value 选项个数：") << command_line_service->variable_count(u8"no_default_value_no_implicit_value") << "\n";
				std::optional<std::reference_wrapper<int>> value = command_line_service->variable<int>(u8"no_default_value_no_implicit_value");
				//可以简化为auto value = ...

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"no_default_value_no_implicit_value 选项的值：");
				if (value)
				{
					std::cout << (*value).get() << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					(*value).get() = 100;

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"no_default_value_no_implicit_value 选项的值：");
					std::cout << (*value).get() << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
			}

            //3.2选项设定 default_value ，但没有设定 implicit_value
            //测试命令行中：
            //a.没有 --has_default_value_no_implicit_value             结果：即便没有在命令行中指定，也因为有default_value，相当于在命令行中有 --has_default_value_no_implicit_value default_value
            //b.--has_default_value_no_implicit_value                  结果：抛出异常
            //c.--has_default_value_no_implicit_value 150              结果：(显而易见)
			{
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"has_default_value_no_implicit_value 选项个数：") << command_line_service->variable_count(u8"has_default_value_no_implicit_value") << "\n";
				std::optional<std::reference_wrapper<int>> value = command_line_service->variable<int>(u8"has_default_value_no_implicit_value");
				//可以简化为auto value = ...

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"has_default_value_no_implicit_value 选项的值：");
				if (value)
				{
					std::cout << (*value).get() << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					(*value).get() = 100;

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"has_default_value_no_implicit_value 选项的值：");
					std::cout << (*value).get() << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
			}




            //3.3选项设定 default_value 和 implicit_value
            //测试命令行中：
            //a.没有 --has_default_value_has_implicit_value             结果：即便没有在命令行中指定，也因为有default_value，相当于在命令行中有 --has_default_value_has_implicit_value default_value
            //b.--has_default_value_has_implicit_value                  结果：相当于 --has_default_value_has_implicit_value implicit_value
            //c.--has_default_value_has_implicit_value 150              结果：(显而易见)
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"has_default_value_has_implicit_value 选项个数：") << command_line_service->variable_count(u8"has_default_value_has_implicit_value") << "\n";
				std::optional<std::reference_wrapper<int>> value = command_line_service->variable<int>(u8"has_default_value_has_implicit_value");
				//可以简化为auto value = ...

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"has_default_value_has_implicit_value 选项的值：");
				if (value)
				{
					std::cout << (*value).get() << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					(*value).get() = 100;

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"has_default_value_has_implicit_value 选项的值：");
					std::cout << (*value).get() << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }


            {
                //has_default_value_has_implicit_value选型对应的值类型是int，如果用错误类型那么返回value是无效的
				std::optional<std::reference_wrapper<float>> value = command_line_service->variable<float>(u8"has_default_value_has_implicit_value");
                if (value)
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"错误，类型不同") << "\n";
                }
            }

            /////////////////不同类型值的选项

            //std::wstring  支持选项值为中文
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_wstring_option_name 选项个数：") << command_line_service->variable_count(u8"custom_wstring_option_name") << "\n";
				std::optional<std::reference_wrapper<std::wstring>> value = command_line_service->variable<std::wstring>(u8"custom_wstring_option_name");
				//可以简化为auto value = ...

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_wstring_option_name 选项的值：");
				if (value)
				{
                    std::wstring& v = (*value).get();

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(v) << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					v = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"这就是修改后的值");


				    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_wstring_option_name 选项的值：");
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(v) << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }

            //short
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_short_option_name 选项个数：") << command_line_service->variable_count(u8"custom_short_option_name") << "\n";
				auto value = command_line_service->variable<short>(u8"custom_short_option_name");

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_short_option_name 选项的值：");
				if (value)
				{
                    short& v = (*value).get();

					std::cout << v << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					v = 6;

				    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_short_option_name 选项的值：");
					std::cout << v << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }

            //unsigned short
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_unsigned_short_option_name 选项个数：") << command_line_service->variable_count(u8"custom_unsigned_short_option_name") << "\n";
				auto value = command_line_service->variable<unsigned short>(u8"custom_unsigned_short_option_name");

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_unsigned_short_option_name 选项的值：");
				if (value)
				{
                    unsigned short& v = (*value).get();

					std::cout << v << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					v = 6;

				    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_unsigned_short_option_name 选项的值：");
					std::cout << v << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }

            //int
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_int_option_name 选项个数：") << command_line_service->variable_count(u8"custom_int_option_name") << "\n";
				auto value = command_line_service->variable<int>(u8"custom_int_option_name");

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_int_option_name 选项的值：");
				if (value)
				{
                    int& v = (*value).get();

					std::cout << v << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					v = 6;

				    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_int_option_name 选项的值：");
					std::cout << v << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }

            //unsigned int
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_unsigned_int_option_name 选项个数：") << command_line_service->variable_count(u8"custom_unsigned_int_option_name") << "\n";
				auto value = command_line_service->variable<unsigned int>(u8"custom_unsigned_int_option_name");

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_unsigned_int_option_name 选项的值：");
				if (value)
				{
                    unsigned int& v = (*value).get();

					std::cout << v << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					v = 6;

				    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_unsigned_int_option_name 选项的值：");
					std::cout << v << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }

            //long long
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_long_long_option_name 选项个数：") << command_line_service->variable_count(u8"custom_long_long_option_name") << "\n";
				auto value = command_line_service->variable<long long>(u8"custom_long_long_option_name");

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_long_long_option_name 选项的值：");
				if (value)
				{
                    long long& v = (*value).get();

					std::cout << v << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					v = 6;

				    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_long_long_option_name 选项的值：");
					std::cout << v << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }

            //unsigned long long
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_unsigned_long_long_option_name 选项个数：") << command_line_service->variable_count(u8"custom_unsigned_long_long_option_name") << "\n";
				auto value = command_line_service->variable<unsigned long long>(u8"custom_unsigned_long_long_option_name");

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_unsigned_long_long_option_name 选项的值：");
				if (value)
				{
                    unsigned long long& v = (*value).get();

					std::cout << v << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					v = 6;

				    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_unsigned_long_long_option_name 选项的值：");
					std::cout << v << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }

            //float
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_float_option_name 选项个数：") << command_line_service->variable_count(u8"custom_float_option_name") << "\n";
				auto value = command_line_service->variable<float>(u8"custom_float_option_name");

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_float_option_name 选项的值：");
				if (value)
				{
                    float& v = (*value).get();

					std::cout << v << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					v = 6.1f;

				    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_float_option_name 选项的值：");
					std::cout << v << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }

            //double
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_double_option_name 选项个数：") << command_line_service->variable_count(u8"custom_double_option_name") << "\n";
				auto value = command_line_service->variable<double>(u8"custom_double_option_name");

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_double_option_name 选项的值：");
				if (value)
				{
                    double& v = (*value).get();

					std::cout << v << "\n";

					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"修改选项值") << "\n";
					v = 6.1;

				    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_double_option_name 选项的值：");
					std::cout << v << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }


            ////相应的数组形式

            //std::vector<std::wstring>  支持选项值为中文
            //比如：命令行中是--custom_vector_wstring_option_name 1 2 3，那么v中的元素就是1，2，3，三个字符串
            {
				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_wstring_option_name 选项个数：") << command_line_service->variable_count(u8"custom_vector_wstring_option_name") << "\n";
				auto value = command_line_service->variable<std::vector<std::wstring>>(u8"custom_vector_wstring_option_name");

				std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_wstring_option_name 选项的值：");
				if (value)
				{
                    std::vector<std::wstring>& v = (*value).get();

                    for (auto element : v)
                    {
                        std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(element)
                            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8", ");
                    }
					std::cout << "\n";
				}
				else
				{
					std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
				}
            }

            //std::vector<short>
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_short_option_name 选项个数：") << command_line_service->variable_count(u8"custom_vector_short_option_name") << "\n";
                auto value = command_line_service->variable<std::vector<short>>(u8"custom_vector_short_option_name");

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_short_option_name 选项的值：");
                if (value)
                {
                    std::vector<short>& v = (*value).get();

                    for (auto element : v)
                    {
                        std::cout << element 
                            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8", ");
                    }
                    std::cout << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }

            //std::vector<unsigned short>
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_unsigned_short_option_name 选项个数：") << command_line_service->variable_count(u8"custom_vector_unsigned_short_option_name") << "\n";
                auto value = command_line_service->variable<std::vector<unsigned short>>(u8"custom_vector_unsigned_short_option_name");

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_unsigned_short_option_name 选项的值：");
                if (value)
                {
                    std::vector<unsigned short>& v = (*value).get();

                    for (auto element : v)
                    {
                        std::cout << element 
                            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8", ");
                    }
                    std::cout << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }

            //std::vector<int>
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_int_option_name 选项个数：") << command_line_service->variable_count(u8"custom_vector_int_option_name") << "\n";
                auto value = command_line_service->variable<std::vector<int>>(u8"custom_vector_int_option_name");

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_int_option_name 选项的值：");
                if (value)
                {
                    std::vector<int>& v = (*value).get();

                    for (auto element : v)
                    {
                        std::cout << element
                            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8", ");
                    }
                    std::cout << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }

            //std::vector<unsigned int>
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_unsigned_int_option_name 选项个数：") << command_line_service->variable_count(u8"custom_vector_unsigned_int_option_name") << "\n";
                auto value = command_line_service->variable<std::vector<unsigned int>>(u8"custom_vector_unsigned_int_option_name");

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_unsigned_int_option_name 选项的值：");
                if (value)
                {
                    std::vector<unsigned int>& v = (*value).get();

                    for (auto element : v)
                    {
                        std::cout << element
                            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8", ");
                    }
                    std::cout << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }

            //std::vector<long long>
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_long_long_option_name 选项个数：") << command_line_service->variable_count(u8"custom_vector_long_long_option_name") << "\n";
                auto value = command_line_service->variable<std::vector<long long>>(u8"custom_vector_long_long_option_name");

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_long_long_option_name 选项的值：");
                if (value)
                {
                    std::vector<long long>& v = (*value).get();

                    for (auto element : v)
                    {
                        std::cout << element
                            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8", ");
                    }
                    std::cout << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }

            //std::vector<unsigned long long>
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_unsigned_long_long_option_name 选项个数：") << command_line_service->variable_count(u8"custom_vector_unsigned_long_long_option_name") << "\n";
                auto value = command_line_service->variable<std::vector<unsigned long long>>(u8"custom_vector_unsigned_long_long_option_name");

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_unsigned_long_long_option_name 选项的值：");
                if (value)
                {
                    std::vector<unsigned long long>& v = (*value).get();

                    for (auto element : v)
                    {
                        std::cout << element
                            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8", ");
                    }
                    std::cout << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }

            //std::vector<float>
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_float_option_name 选项个数：") << command_line_service->variable_count(u8"custom_vector_float_option_name") << "\n";
                auto value = command_line_service->variable<std::vector<float>>(u8"custom_vector_float_option_name");

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_float_option_name 选项的值：");
                if (value)
                {
                    std::vector<float>& v = (*value).get();

                    for (auto element : v)
                    {
                        std::cout << element
                            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8", ");
                    }
                    std::cout << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }

            //std::vector<double>
            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_double_option_name 选项个数：") << command_line_service->variable_count(u8"custom_vector_double_option_name") << "\n";
                auto value = command_line_service->variable<std::vector<double>>(u8"custom_vector_double_option_name");

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_vector_double_option_name 选项的值：");
                if (value)
                {
                    std::vector<double>& v = (*value).get();

                    for (auto element : v)
                    {
                        std::cout << element
                            << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8", ");
                    }
                    std::cout << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }

            ////bool类型比较特殊

            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_bool_switch_option_name 选项个数：") << command_line_service->variable_count(u8"custom_bool_switch_option_name") << "\n";
                std::optional<std::reference_wrapper<bool>> value = command_line_service->variable<bool>(u8"custom_bool_switch_option_name");
                //可以简化为auto value = ...

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_bool_switch_option_name 选项的值：");
                if (value)
                {
                    bool& v = (*value).get();
                    std::cout << std::boolalpha << v << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }

            {
                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_true_bool_switch_option_name 选项个数：") << command_line_service->variable_count(u8"custom_true_bool_switch_option_name") << "\n";
                std::optional<std::reference_wrapper<bool>> value = command_line_service->variable<bool>(u8"custom_true_bool_switch_option_name");
                //可以简化为auto value = ...

                std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"custom_true_bool_switch_option_name 选项的值：");
                if (value)
                {
                    bool& v = (*value).get();
                    std::cout << std::boolalpha << v << "\n";
                }
                else
                {
                    std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"(没有值)") << "\n";
                }
            }



            //判断是否制定了选项
            std::cout << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(u8"variable empty：") << std::boolalpha << command_line_service->variable_empty() << "\n";


            for (int i = 0; i < argc; ++i)
            {
                free(argv_[i]);
            }
            delete[] argv_;

            return 0;
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
    //特别注意！MSYS64环境要求main函数正常返回必须用"return 0"，不能省略他否则崩溃
}