#include <kiwi.fast.utility/src/register_type.h>

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/value_converter.h>
#include <kiwi.fast.plugin_utility/service_object_factory.h>
#include <kiwi.fast.plugin_utility/service_data_value.h>

#include <kiwi.fast.utility/src/service_object_factory_imp.h>
#include <kiwi.fast.utility/src/service_data_value_imp.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

namespace
{
	template<typename T, typename TImp = T>
	void insert_object_create_destroy()
	{
		service_object_factory_imp::insert_object_create_destroy<T, TImp>();
		service_data_value_imp::insert_object_create_destroy<T, TImp>();
	};
	template<typename T, typename TImp = T>
	void insert_deque_create_destroy()
	{
		service_object_factory_imp::insert_deque_create_destroy<T, TImp>();
		service_data_value_imp::insert_deque_create_destroy<T, TImp>();
	};

	template<typename T, typename FToString, typename FFromString>
	static void insert_object_u8string(FToString f_to_string, FFromString f_from_string)
	{
		service_object_factory_imp::insert_object_u8string<T, FToString, FFromString>(f_to_string, f_from_string);
		service_data_value_imp::insert_object_u8string<T, FToString, FFromString>(f_to_string, f_from_string);
	}

	template<typename T, typename FToString, typename FFromString>
	static void insert_deque_u8string(FToString f_to_string, FFromString f_from_string)
	{
		service_object_factory_imp::insert_deque_u8string<T, FToString, FFromString>(f_to_string, f_from_string);
		service_data_value_imp::insert_deque_u8string<T, FToString, FFromString>(f_to_string, f_from_string);
	}
}

register_type* register_type::m_instance = nullptr;
bool register_type::m_destroyed = false;

register_type::deleter register_type::m_deleter;

std::u8string register_type::m_separation = u8"‖";//分隔符号

register_type::register_type()
{
	//bool*
	insert_object_create_destroy<bool>();
	insert_object_u8string<bool>(
		[](bool const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
			return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
		}
		, [](std::u8string const& str, bool* object) {
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
		});

	//std::deque<bool*>*
	insert_deque_create_destroy<bool>();
	insert_deque_u8string<bool>(
		[](std::deque<bool*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
			return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
		}
		, [this](std::u8string const& str, std::deque<bool*>* deque) {
			std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
			std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<bool>();
			for (int i = 0; i < element_number; ++i)
			{
				KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
				deque->push_back(std::any_cast<bool*>(object_factory_service->create_object(class_name)));
			}
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
		});


    //char*
    insert_object_create_destroy<char>();
    insert_object_u8string<char>(
        [](char const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, char* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<char*>*
    insert_deque_create_destroy<char>();
    insert_deque_u8string<char>(
        [](std::deque<char*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<char*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<char>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<char*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //unsigned char*
    insert_object_create_destroy<unsigned char>();
    insert_object_u8string<unsigned char>(
        [](unsigned char const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, unsigned char* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<unsigned char*>*
    insert_deque_create_destroy<unsigned char>();
    insert_deque_u8string<unsigned char>(
        [](std::deque<unsigned char*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<unsigned char*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<unsigned char>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<unsigned char*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //wchar_t 不同平台下长度不同，所以不提供
    
    //long 不同平台下长度不同，所以不提供

    //signed char
    insert_object_create_destroy<signed char>();
    insert_object_u8string<signed char>(
        [](signed char const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, signed char* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<signed char*>*
    insert_deque_create_destroy<signed char>();
    insert_deque_u8string<signed char>(
        [](std::deque<signed char*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<signed char*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<signed char>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<signed char*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //char8_t*
    insert_object_create_destroy<char8_t>();
    insert_object_u8string<char8_t>(
        [](char8_t const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, char8_t* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<char8_t*>*
    insert_deque_create_destroy<char8_t>();
    insert_deque_u8string<char8_t>(
        [](std::deque<char8_t*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<char8_t*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<char8_t>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<char8_t*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //std::string*
    insert_object_create_destroy<std::string>();
    insert_object_u8string<std::string>(
        [](std::string const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, std::string* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<std::string*>*
    insert_deque_create_destroy<std::string>();
    insert_deque_u8string<std::string>(
        [](std::deque<std::string*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<std::string*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::string>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<std::string*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //std::wstring*
    insert_object_create_destroy<std::wstring>();
    insert_object_u8string<std::wstring>(
        [](std::wstring const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, std::wstring* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<std::wstring*>*
    insert_deque_create_destroy<std::wstring>();
    insert_deque_u8string<std::wstring>(
        [](std::deque<std::wstring*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<std::wstring*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::wstring>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<std::wstring*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //std::u8string*
    insert_object_create_destroy<std::u8string>();
    insert_object_u8string<std::u8string>(
        [](std::u8string const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, std::u8string* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<std::u8string*>*
    insert_deque_create_destroy<std::u8string>();
    insert_deque_u8string<std::u8string>(
        [](std::deque<std::u8string*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<std::u8string*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::u8string>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<std::u8string*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //short
    insert_object_create_destroy<short>();
    insert_object_u8string<short>(
        [](short const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, short* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<short*>*
    insert_deque_create_destroy<short>();
    insert_deque_u8string<short>(
        [](std::deque<short*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<short*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<short>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<short*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //unsigned short*
    insert_object_create_destroy<unsigned short>();
    insert_object_u8string<unsigned short>(
        [](unsigned short const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, unsigned short* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<unsigned short*>*
    insert_deque_create_destroy<unsigned short>();
    insert_deque_u8string<unsigned short>(
        [](std::deque<unsigned short*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<unsigned short*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<unsigned short>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<unsigned short*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //int*
    insert_object_create_destroy<int>();
    insert_object_u8string<int>(
        [](int const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, int* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<int*>*
    insert_deque_create_destroy<int>();
    insert_deque_u8string<int>(
        [](std::deque<int*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<int*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<int>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<int*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //unsigned int*
    insert_object_create_destroy<unsigned int>();
    insert_object_u8string<unsigned int>(
        [](unsigned int const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, unsigned int* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<unsigned int*>*
    insert_deque_create_destroy<unsigned int>();
    insert_deque_u8string<unsigned int>(
        [](std::deque<unsigned int*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<unsigned int*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<unsigned int>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<unsigned int*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //long long*
    insert_object_create_destroy<long long>();
    insert_object_u8string<long long>(
        [](long long const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, long long* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<long long*>*
    insert_deque_create_destroy<long long>();
    insert_deque_u8string<long long>(
        [](std::deque<long long*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<long long*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<long long>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<long long*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //unsigned long long*
    insert_object_create_destroy<unsigned long long>();
    insert_object_u8string<unsigned long long>(
        [](unsigned long long const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, unsigned long long* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<unsigned long long*>*
    insert_deque_create_destroy<unsigned long long>();
    insert_deque_u8string<unsigned long long>(
        [](std::deque<unsigned long long*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<unsigned long long*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<unsigned long long>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<unsigned long long*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //float*
    insert_object_create_destroy<float>();
    insert_object_u8string<float>(
        [](float const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, float* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<float*>*
    insert_deque_create_destroy<float>();
    insert_deque_u8string<float>(
        [](std::deque<float*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<float*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<float>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<float*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //double*
    insert_object_create_destroy<double>();
    insert_object_u8string<double>(
        [](double const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, double* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<double*>*
    insert_deque_create_destroy<double>();
    insert_deque_u8string<double>(
        [](std::deque<double*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<double*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<double>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<double*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //std::filesystem::path*
    insert_object_create_destroy<std::filesystem::path>();
    insert_object_u8string<std::filesystem::path>(
        [](std::filesystem::path const* object) {
            if (object == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_to_string(object);
        }
        , [](std::u8string const& str, std::filesystem::path* object) {
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::object_from_string(object, str);
        });

    //std::deque<std::filesystem::path*>*
    insert_deque_create_destroy<std::filesystem::path>();
    insert_deque_u8string<std::filesystem::path>(
        [](std::deque<std::filesystem::path*> const* deque) {
            if (deque == nullptr)
            {
                return std::optional<std::u8string>();
            }
            return KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_to_string(deque, register_type::m_separation);
        }
        , [](std::u8string const& str, std::deque<std::filesystem::path*>* deque) {
            std::size_t element_number = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::detail::deque_element_number_from_string(str, register_type::m_separation);
            std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::filesystem::path>();
            for (int i = 0; i < element_number; ++i)
            {
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque->push_back(std::any_cast<std::filesystem::path*>(object_factory_service->create_object(class_name)));
            }
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER value_converter::deque_from_string(deque, str, register_type::m_separation);
        });

    //...
}

register_type::~register_type()
{}

KIWI_FAST_CLOSE_UTILITY_NAMESPACE