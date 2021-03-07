#include <kiwi.fast.utility/src/service_object_factory_imp.h>

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/type_converter.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_object_factory_imp::u8string_create_object_function_map_type service_object_factory_imp::m_create_object_map;
service_object_factory_imp::u8string_destroy_object_function_map_type service_object_factory_imp::m_destroy_object_map;
service_object_factory_imp::u8string_create_deque_function_map_type service_object_factory_imp::m_create_deque_map;
service_object_factory_imp::u8string_destroy_deque_function_map_type service_object_factory_imp::m_destroy_deque_map;

service_object_factory_imp::service_object_factory_imp()
{
    ////对应type_converter中的类型进行添加，此外再添加其他自定义类型

    //bool
    insert_object_create_destroy<bool>();
    //std::deque<bool>
    insert_deque_create_destroy<bool>();

    //char
    insert_object_create_destroy<char>();
    //std::deque<char>
    insert_deque_create_destroy<char>();

    //unsigned char
    insert_object_create_destroy<unsigned char>();
    //std::deque<unsigned char>
    insert_deque_create_destroy<unsigned char>();

    //signed char
    insert_object_create_destroy<signed char>();
    //std::deque<signed char>
    insert_deque_create_destroy<signed char>();

    //wchar_t
    insert_object_create_destroy<wchar_t>();
    //std::deque<wchar_t>
    insert_deque_create_destroy<wchar_t>();

    //char8_t
    insert_object_create_destroy<char8_t>();
    //std::deque<char8_t>
    insert_deque_create_destroy<char8_t>();

    //std::string
    insert_object_create_destroy<std::string>();
    //std::deque<std::string>
    insert_deque_create_destroy<std::string>();

    //std::wstring
    insert_object_create_destroy<std::wstring>();
    //std::deque<std::wstring>
    insert_deque_create_destroy<std::wstring>();

    //std::u8string
    insert_object_create_destroy<std::u8string>();
    //std::deque<std::u8string>
    insert_deque_create_destroy<std::u8string>();

    //short
    insert_object_create_destroy<short>();
    //std::deque<short>
    insert_deque_create_destroy<short>();

    //unsigned short
    insert_object_create_destroy<unsigned short>();
    //std::deque<unsigned short>
    insert_deque_create_destroy<unsigned short>();

    //int
    insert_object_create_destroy<int>();
    //std::deque<int>
    insert_deque_create_destroy<int>();

    //unsigned int
    insert_object_create_destroy<unsigned int>();
    //std::deque<unsigned int>
    insert_deque_create_destroy<unsigned int>();

    //long long
    insert_object_create_destroy<long long>();
    //std::deque<long long>
    insert_deque_create_destroy<long long>();

    //unsigned long long
    insert_object_create_destroy<unsigned long long>();
    //std::deque<unsigned long long>
    insert_deque_create_destroy<unsigned long long>();

    //float
    insert_object_create_destroy<float>();
    //std::deque<float>
    insert_deque_create_destroy<float>();

    //double
    insert_object_create_destroy<double>();
    //std::deque<double>
    insert_deque_create_destroy<double>();

    //std::filesystem::path
    insert_object_create_destroy<std::filesystem::path>();
    //std::deque<std::filesystem::path>
    insert_deque_create_destroy<std::filesystem::path>();

    //std::any
    insert_object_create_destroy<std::any>();
    //std::deque<std::any>
    insert_deque_create_destroy<std::any>();

    //...
}

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
