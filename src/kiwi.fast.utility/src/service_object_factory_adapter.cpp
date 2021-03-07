#include <kiwi.fast.utility/src/service_object_factory_adapter.h>

#include <kiwi.fast.utility/detail/config.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_object_factory_adapter* service_object_factory_adapter::m_instance = nullptr;
bool service_object_factory_adapter::m_destroyed = false;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
