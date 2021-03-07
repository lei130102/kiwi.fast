#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.utility/src/service_setting_adapter.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_setting_adapter* service_setting_adapter::m_instance = nullptr;
bool service_setting_adapter::m_destroyed = false;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
