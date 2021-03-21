#include <kiwi.fast.utility/src/service_data_value_adapter.h>

#include <kiwi.fast.utility/detail/config.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_data_value_adapter* service_data_value_adapter::m_instance = nullptr;
bool service_data_value_adapter::m_destroyed = false;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE