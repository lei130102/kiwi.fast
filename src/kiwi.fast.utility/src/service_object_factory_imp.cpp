#include <kiwi.fast.utility/src/service_object_factory_imp.h>

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/value_converter.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_object_factory_imp::u8string_create_object_function_map_type service_object_factory_imp::m_create_object_map;
service_object_factory_imp::u8string_destroy_object_function_map_type service_object_factory_imp::m_destroy_object_map;

service_object_factory_imp::u8string_create_deque_function_map_type service_object_factory_imp::m_create_deque_map;
service_object_factory_imp::u8string_destroy_deque_function_map_type service_object_factory_imp::m_destroy_deque_map;

service_object_factory_imp::u8string_object_u8string_function_map_type service_object_factory_imp::m_object_u8string_map;
service_object_factory_imp::u8string_deque_u8string_function_map_type service_object_factory_imp::m_deque_u8string_map;

service_object_factory_imp::service_object_factory_imp()
{
    ////对应type_converter中的类型进行添加，此外再添加其他自定义类型

    //bool
    insert_object_create_destroy<bool>();
    insert_object_u8string<bool, bool>(
                [](bool* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](bool* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, bool>(str);
    });

    //std::deque<bool>
    insert_deque_create_destroy<bool>();
    insert_deque_u8string<bool, bool>(
                [](std::deque<bool>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, bool>(*deque, u8", ");
    }
                , [](std::deque<bool>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, bool>(str, u8", ");
    });

    //char
    insert_object_create_destroy<char>();
    insert_object_u8string<char, char>(
                [](char* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](char* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, char>(str);
    });

    //std::deque<char>
    insert_deque_create_destroy<char>();
    insert_deque_u8string<char, char>(
                [](std::deque<char>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, char>(*deque, u8", ");
    }
                , [](std::deque<char>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, char>(str, u8", ");
    });

    //unsigned char
    insert_object_create_destroy<unsigned char>();
    insert_object_u8string<unsigned char, unsigned char>(
                [](unsigned char* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](unsigned char* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned char>(str);
    });

    //std::deque<unsigned char>
    insert_deque_create_destroy<unsigned char>();
    insert_deque_u8string<unsigned char, unsigned char>(
                [](std::deque<unsigned char>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned char>(*deque, u8", ");
    }
                , [](std::deque<unsigned char>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned char>(str, u8", ");
    });

    //signed char
    insert_object_create_destroy<signed char>();
    insert_object_u8string<signed char, signed char>(
                [](signed char* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](signed char* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, signed char>(str);
    });

    //std::deque<signed char>
    insert_deque_create_destroy<signed char>();
    insert_deque_u8string<signed char, signed char>(
                [](std::deque<signed char>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, signed char>(*deque, u8", ");
    }
                , [](std::deque<signed char>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, signed char>(str, u8", ");
    });

    //wchar_t
    insert_object_create_destroy<wchar_t>();
    insert_object_u8string<wchar_t, wchar_t>(
                [](wchar_t* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](wchar_t* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, wchar_t>(str);
    });

    //std::deque<wchar_t>
    insert_deque_create_destroy<wchar_t>();
    insert_deque_u8string<wchar_t, wchar_t>(
                [](std::deque<wchar_t>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, wchar_t>(*deque, u8", ");
    }
                , [](std::deque<wchar_t>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, wchar_t>(str, u8", ");
    });

    //char8_t
    insert_object_create_destroy<char8_t>();
    insert_object_u8string<char8_t, char8_t>(
                [](char8_t* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](char8_t* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, char8_t>(str);
    });

    //std::deque<char8_t>
    insert_deque_create_destroy<char8_t>();
    insert_deque_u8string<char8_t, char8_t>(
                [](std::deque<char8_t>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, char8_t>(*deque, u8", ");
    }
                , [](std::deque<char8_t>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, char8_t>(str, u8", ");
    });

    //std::string
    insert_object_create_destroy<std::string>();
    insert_object_u8string<std::string, std::string>(
                [](std::string* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](std::string* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, std::string>(str);
    });

    //std::deque<std::string>
    insert_deque_create_destroy<std::string>();
    insert_deque_u8string<std::string, std::string>(
                [](std::deque<std::string>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, std::string>(*deque, u8", ");
    }
                , [](std::deque<std::string>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, std::string>(str, u8", ");
    });

    //不提供std::wstring std::deque<std::wstring> std::u8string std::deque<std::u8string>

    //short
    insert_object_create_destroy<short>();
    insert_object_u8string<short, short>(
                [](short* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](short* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, short>(str);
    });

    //std::deque<short>
    insert_deque_create_destroy<short>();
    insert_deque_u8string<short, short>(
                [](std::deque<short>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, short>(*deque, u8", ");
    }
                , [](std::deque<short>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, short>(str, u8", ");
    });

    //unsigned short
    insert_object_create_destroy<unsigned short>();
    insert_object_u8string<unsigned short, unsigned short>(
                [](unsigned short* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](unsigned short* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned short>(str);
    });

    //std::deque<unsigned short>
    insert_deque_create_destroy<unsigned short>();
    insert_deque_u8string<unsigned short, unsigned short>(
                [](std::deque<unsigned short>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned short>(*deque, u8", ");
    }
                , [](std::deque<unsigned short>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned short>(str, u8", ");
    });

    //int
    insert_object_create_destroy<int>();
    insert_object_u8string<int, int>(
                [](int* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](int* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, int>(str);
    });

    //std::deque<int>
    insert_deque_create_destroy<int>();
    insert_deque_u8string<int, int>(
                [](std::deque<int>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, int>(*deque, u8", ");
    }
                , [](std::deque<int>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, int>(str, u8", ");
    });

    //unsigned int
    insert_object_create_destroy<unsigned int>();
    insert_object_u8string<unsigned int, unsigned int>(
                [](unsigned int* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](unsigned int* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned int>(str);
    });

    //std::deque<unsigned int>
    insert_deque_create_destroy<unsigned int>();
    insert_deque_u8string<unsigned int, unsigned int>(
                [](std::deque<unsigned int>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned int>(*deque, u8", ");
    }
                , [](std::deque<unsigned int>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned int>(str, u8", ");
    });

    //long long
    insert_object_create_destroy<long long>();
    insert_object_u8string<long long, long long>(
                [](long long* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](long long* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, long long>(str);
    });

    //std::deque<long long>
    insert_deque_create_destroy<long long>();
    insert_deque_u8string<long long, long long>(
                [](std::deque<long long>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, long long>(*deque, u8", ");
    }
                , [](std::deque<long long>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, long long>(str, u8", ");
    });

    //unsigned long long
    insert_object_create_destroy<unsigned long long>();
    insert_object_u8string<unsigned long long, unsigned long long>(
                [](unsigned long long* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](unsigned long long* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned long long>(str);
    });

    //std::deque<unsigned long long>
    insert_deque_create_destroy<unsigned long long>();
    insert_deque_u8string<unsigned long long, unsigned long long>(
                [](std::deque<unsigned long long>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned long long>(*deque, u8", ");
    }
                , [](std::deque<unsigned long long>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, unsigned long long>(str, u8", ");
    });

    //float
    insert_object_create_destroy<float>();
    insert_object_u8string<float, float>(
                [](float* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](float* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, float>(str);
    });

    //std::deque<float>
    insert_deque_create_destroy<float>();
    insert_deque_u8string<float, float>(
                [](std::deque<float>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, float>(*deque, u8", ");
    }
                , [](std::deque<float>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, float>(str, u8", ");
    });

    //double
    insert_object_create_destroy<double>();
    insert_object_u8string<double, double>(
                [](double* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](double* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, double>(str);
    });

    //std::deque<double>
    insert_deque_create_destroy<double>();
    insert_deque_u8string<double, double>(
                [](std::deque<double>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, double>(*deque, u8", ");
    }
                , [](std::deque<double>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, double>(str, u8", ");
    });

    //std::filesystem::path
    insert_object_create_destroy<std::filesystem::path>();
    insert_object_u8string<std::filesystem::path, std::filesystem::path>(
                [](std::filesystem::path* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display>(object);
    }
                , [](std::filesystem::path* object, std::u8string const& str){
        *object = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::from_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, std::filesystem::path>(str);
    });

    //std::deque<std::filesystem::path>
    insert_deque_create_destroy<std::filesystem::path>();
    insert_deque_u8string<std::filesystem::path, std::filesystem::path>(
                [](std::deque<std::filesystem::path>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, std::filesystem::path>(*deque, u8", ");
    }
                , [](std::deque<std::filesystem::path>* deque, std::u8string const& str){
        *deque = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::string_to_deque<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::display, std::filesystem::path>(str, u8", ");
    });

    //std::any
    insert_object_create_destroy<std::any>();
    //std::deque<std::any>
    insert_deque_create_destroy<std::any>();

    //...
}

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
