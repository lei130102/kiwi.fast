#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.utility/src/service_command_line_adapter.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

service_command_line_adapter* service_command_line_adapter::m_instance = nullptr;
bool service_command_line_adapter::m_destroyed = false;

KIWI_FAST_CLOSE_UTILITY_NAMESPACE
