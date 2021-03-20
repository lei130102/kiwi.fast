#include <kiwi.fast.utility/src/service_object_factory_imp.h>

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/value_converter.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_object_factory_imp::u8string_create_object_function_map_type service_object_factory_imp::m_create_object_map;
service_object_factory_imp::u8string_destroy_object_function_map_type service_object_factory_imp::m_destroy_object_map;

service_object_factory_imp::u8string_create_deque_function_map_type service_object_factory_imp::m_create_deque_map;
service_object_factory_imp::u8string_destroy_deque_function_map_type service_object_factory_imp::m_destroy_deque_map;

service_object_factory_imp::u8string_create_resource_object_function_map_type service_object_factory_imp::m_create_resource_object_map;
service_object_factory_imp::u8string_create_resource_deque_function_map_type service_object_factory_imp::m_create_resource_deque_map;

service_object_factory_imp::u8string_object_u8string_function_map_type service_object_factory_imp::m_object_u8string_map;
service_object_factory_imp::u8string_deque_u8string_function_map_type service_object_factory_imp::m_deque_u8string_map;

service_object_factory_imp::u8string_resource_object_u8string_function_map_type service_object_factory_imp::m_resource_object_u8string_map;
service_object_factory_imp::u8string_resource_deque_u8string_function_map_type service_object_factory_imp::m_resource_deque_u8string_map;

service_object_factory_imp::service_object_factory_imp()
{
    ////对应type_converter中的类型进行添加，此外再添加其他自定义类型

    //bool*
    insert_object_create_destroy<bool>();
    insert_object_u8string<bool>(
                [](bool* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](bool* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<bool*>*
    insert_deque_create_destroy<bool>();
    insert_deque_u8string<bool>(
                [](std::deque<bool*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [this](std::deque<bool*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<bool*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //char*
    insert_object_create_destroy<char>();
    insert_object_u8string<char>(
                [](char* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](char* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<char*>*
    insert_deque_create_destroy<char>();
    insert_deque_u8string<char>(
                [](std::deque<char*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<char*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<char>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<char*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //unsigned char*
    insert_object_create_destroy<unsigned char>();
    insert_object_u8string<unsigned char>(
                [](unsigned char* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](unsigned char* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<unsigned char*>*
    insert_deque_create_destroy<unsigned char>();
    insert_deque_u8string<unsigned char>(
                [](std::deque<unsigned char*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<unsigned char*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<unsigned char>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<unsigned char*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //signed char
    insert_object_create_destroy<signed char>();
    insert_object_u8string<signed char>(
                [](signed char* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](signed char* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<signed char*>*
    insert_deque_create_destroy<signed char>();
    insert_deque_u8string<signed char>(
                [](std::deque<signed char*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<signed char*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<signed char>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<signed char*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //wchar_t*
    insert_object_create_destroy<wchar_t>();
    insert_object_u8string<wchar_t>(
                [](wchar_t* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](wchar_t* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<wchar_t*>*
    insert_deque_create_destroy<wchar_t>();
    insert_deque_u8string<wchar_t>(
                [](std::deque<wchar_t*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<wchar_t*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<wchar_t>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<wchar_t*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //char8_t*
    insert_object_create_destroy<char8_t>();
    insert_object_u8string<char8_t>(
                [](char8_t* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](char8_t* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<char8_t*>*
    insert_deque_create_destroy<char8_t>();
    insert_deque_u8string<char8_t>(
                [](std::deque<char8_t*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<char8_t*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<char8_t>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<char8_t*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //std::string*
    insert_object_create_destroy<std::string>();
    insert_object_u8string<std::string>(
                [](std::string* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](std::string* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<std::string*>*
    insert_deque_create_destroy<std::string>();
    insert_deque_u8string<std::string>(
                [](std::deque<std::string*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<std::string*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::string>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<std::string*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //std::wstring*
    insert_object_create_destroy<std::wstring>();
    insert_object_u8string<std::wstring>(
        [](std::wstring* object) {
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::wstring* object, std::u8string const& str) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<std::wstring*>*
    insert_deque_create_destroy<std::wstring>();
    insert_deque_u8string<std::wstring>(
        [](std::deque<std::wstring*>* deque) {
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
        }
        , [](std::deque<std::wstring*>* deque, std::u8string const& str) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::wstring>();
            for (int i = 0; i < element_number; ++i)
            {
                auto iter = m_create_object_map.find(class_name);
                if (iter != m_create_object_map.end())
                {
                    deque->push_back(std::any_cast<std::wstring*>(((*iter).second)()));
                }
                else
                {
                    KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
                }
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
        });

    //std::u8string*
    insert_object_create_destroy<std::u8string>();
    insert_object_u8string<std::u8string>(
        [](std::u8string* object) {
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string* object, std::u8string const& str) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<std::u8string*>*
    insert_deque_create_destroy<std::u8string>();
    insert_deque_u8string<std::u8string>(
        [](std::deque<std::u8string*>* deque) {
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
        }
        , [](std::deque<std::u8string*>* deque, std::u8string const& str) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::u8string>();
            for (int i = 0; i < element_number; ++i)
            {
                auto iter = m_create_object_map.find(class_name);
                if (iter != m_create_object_map.end())
                {
                    deque->push_back(std::any_cast<std::u8string*>(((*iter).second)()));
                }
                else
                {
                    KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
                }
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
        });

    //short
    insert_object_create_destroy<short>();
    insert_object_u8string<short>(
                [](short* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](short* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<short*>*
    insert_deque_create_destroy<short>();
    insert_deque_u8string<short>(
                [](std::deque<short*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<short*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<short>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<short*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //unsigned short*
    insert_object_create_destroy<unsigned short>();
    insert_object_u8string<unsigned short>(
                [](unsigned short* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](unsigned short* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<unsigned short*>*
    insert_deque_create_destroy<unsigned short>();
    insert_deque_u8string<unsigned short>(
                [](std::deque<unsigned short*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<unsigned short*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<unsigned short>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<unsigned short*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //int*
    insert_object_create_destroy<int>();
    insert_object_u8string<int>(
                [](int* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](int* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<int*>*
    insert_deque_create_destroy<int>();
    insert_deque_u8string<int>(
                [](std::deque<int*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<int*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<int>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<int*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //unsigned int*
    insert_object_create_destroy<unsigned int>();
    insert_object_u8string<unsigned int>(
                [](unsigned int* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](unsigned int* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<unsigned int*>*
    insert_deque_create_destroy<unsigned int>();
    insert_deque_u8string<unsigned int>(
                [](std::deque<unsigned int*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<unsigned int*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<unsigned int>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<unsigned int*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //long long*
    insert_object_create_destroy<long long>();
    insert_object_u8string<long long>(
                [](long long* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](long long* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<long long*>*
    insert_deque_create_destroy<long long>();
    insert_deque_u8string<long long>(
                [](std::deque<long long*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<long long*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<long long>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<long long*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //unsigned long long*
    insert_object_create_destroy<unsigned long long>();
    insert_object_u8string<unsigned long long>(
                [](unsigned long long* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](unsigned long long* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<unsigned long long*>*
    insert_deque_create_destroy<unsigned long long>();
    insert_deque_u8string<unsigned long long>(
                [](std::deque<unsigned long long*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<unsigned long long*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<unsigned long long>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<unsigned long long*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //float*
    insert_object_create_destroy<float>();
    insert_object_u8string<float>(
                [](float* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](float* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<float*>*
    insert_deque_create_destroy<float>();
    insert_deque_u8string<float>(
                [](std::deque<float*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<float*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<float>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<float*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //double*
    insert_object_create_destroy<double>();
    insert_object_u8string<double>(
                [](double* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](double* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<double*>*
    insert_deque_create_destroy<double>();
    insert_deque_u8string<double>(
                [](std::deque<double*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<double*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<double>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<double*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //std::filesystem::path*
    insert_object_create_destroy<std::filesystem::path>();
    insert_object_u8string<std::filesystem::path>(
                [](std::filesystem::path* object){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
    }
                , [](std::filesystem::path* object, std::u8string const& str){
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
    });

    //std::deque<std::filesystem::path*>*
    insert_deque_create_destroy<std::filesystem::path>();
    insert_deque_u8string<std::filesystem::path>(
                [](std::deque<std::filesystem::path*>* deque){
        return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, u8", ");
    }
                , [](std::deque<std::filesystem::path*>* deque, std::u8string const& str){
        std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str.c_str(), u8", ");
        std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::filesystem::path>();
        for (int i = 0; i < element_number; ++i)
        {
            auto iter = m_create_object_map.find(class_name);
            if (iter != m_create_object_map.end())
            {
                deque->push_back(std::any_cast<std::filesystem::path*>(((*iter).second)()));
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过创建对象：") + class_name);
            }
        }
        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str.c_str(), u8", ");
    });

    //...
}

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
