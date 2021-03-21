#include <kiwi.fast.plugin_utility/data_value.h>

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/service_data_value.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

data_value data_value::create(const char8_t* class_name)
{
	service<service_data_value> data_value_service;
	return data_value_service->create_data_value(class_name);
}

std::optional<std::u8string> data_value::to_string() const
{
	service<service_data_value> data_value_service;
	return data_value_service->data_value_to_string(*this);
}

data_value data_value::from_string(const char8_t* str, const char8_t* class_name)
{
	data_value dv = create(class_name);

	service<service_data_value> data_value_service;
	data_value_service->data_value_from_string(str, class_name, dv);

	return dv;
}


KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE