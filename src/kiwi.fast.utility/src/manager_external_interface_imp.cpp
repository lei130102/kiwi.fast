#include <kiwi.fast.utility/src/manager_external_interface_imp.h>

#include <kiwi.fast.utility/detail/config.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

manager_external_interface_imp* manager_external_interface_imp::m_instance = nullptr;
bool manager_external_interface_imp::m_destroyed = false;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
