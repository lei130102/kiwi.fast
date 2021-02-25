#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.utility/threadpool.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

thread_local int threadpool_per_cpu::m_cpu_mask = 0;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
