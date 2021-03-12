#include <kiwi.fast.utility/src/service_log_adapter.h>

#include <kiwi.fast.utility/detail/config.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_log_adapter* service_log_adapter::m_instance = nullptr;
bool service_log_adapter::m_destroyed = false;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
