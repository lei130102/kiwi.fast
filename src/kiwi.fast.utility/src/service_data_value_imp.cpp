#include <kiwi.fast.utility/src/service_data_value_imp.h>

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/value_converter.h>

#include <kiwi.fast.plugin_utility/service_object_factory.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_data_value_imp::u8string_create_data_value_function_map_type service_data_value_imp::m_create_data_object_value_map;

service_data_value_imp::u8string_create_data_value_function_map_type service_data_value_imp::m_create_data_deque_value_map;

//

service_data_value_imp::data_value_to_u8string_function_map_type service_data_value_imp::m_data_value_object_to_u8string_map;
service_data_value_imp::data_value_from_u8string_function_map_type service_data_value_imp::m_data_value_object_from_u8string_map;

service_data_value_imp::data_value_to_u8string_function_map_type service_data_value_imp::m_data_value_deque_to_u8string_map;
service_data_value_imp::data_value_from_u8string_function_map_type service_data_value_imp::m_data_value_deque_from_u8string_map;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE