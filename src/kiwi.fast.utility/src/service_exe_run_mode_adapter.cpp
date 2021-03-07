#include <kiwi.fast.utility/src/service_exe_run_mode_adapter.h>

#include <kiwi.fast.utility/detail/config.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_exe_run_mode_adapter* service_exe_run_mode_adapter::m_instance = nullptr;
bool service_exe_run_mode_adapter::m_destroyed = false;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
