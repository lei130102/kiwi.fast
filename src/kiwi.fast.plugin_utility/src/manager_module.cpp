#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/manager_module.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

manager_module* manager_module::m_singleton = nullptr;

manager_module::deleter manager_module::m_deleter;

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
