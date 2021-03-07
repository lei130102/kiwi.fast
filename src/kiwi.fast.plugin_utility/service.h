#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/manager_module.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

template<typename T>
class service
{
public:
    T* get() const
    {
        T* p = nullptr;
        manager_module::instance()->external_interface_manager()->query(
                    type_converter::to_string<T>()
                    , reinterpret_cast<void**>(&p));
        return p;
    }

    T* operator->() const
    {
        return get();
    }
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
