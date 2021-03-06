#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

template<typename T>
class service
{
public:
    T* get() const;

    T* operator->() const
    {
        return get();
    }
};

class service_command_line;
class service_setting;

template<> service_command_line* service<service_command_line>::get() const;
template<> service_setting* service<service_setting>::get() const;

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
