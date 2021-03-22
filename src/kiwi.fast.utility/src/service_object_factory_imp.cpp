#include <kiwi.fast.utility/src/service_object_factory_imp.h>

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/value_converter.h>

#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/service_object_factory.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_object_factory_imp::u8string_create_function_map_type service_object_factory_imp::m_create_object_map;
service_object_factory_imp::u8string_copy_function_map_type service_object_factory_imp::m_copy_object_map;
service_object_factory_imp::u8string_destroy_function_map_type service_object_factory_imp::m_destroy_object_map;

service_object_factory_imp::u8string_create_function_map_type service_object_factory_imp::m_create_deque_map;
service_object_factory_imp::u8string_copy_function_map_type service_object_factory_imp::m_copy_deque_map;
service_object_factory_imp::u8string_destroy_function_map_type service_object_factory_imp::m_destroy_deque_map;

//

service_object_factory_imp::to_u8string_function_map_type service_object_factory_imp::m_object_to_u8string_map;
service_object_factory_imp::from_u8string_function_map_type service_object_factory_imp::m_object_from_u8string_map;

service_object_factory_imp::to_u8string_function_map_type service_object_factory_imp::m_deque_to_u8string_map;
service_object_factory_imp::from_u8string_function_map_type service_object_factory_imp::m_deque_from_u8string_map;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
