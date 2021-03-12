#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

template<typename T>
class reset_variable_on_destroy
{
public:
    reset_variable_on_destroy(T& variable, T const& value)
        : m_variable(variable)
        , m_original_value(variable)
    {
        m_variable = value;
    }

    reset_variable_on_destroy& operator=(reset_variable_on_destroy const& rhs) = delete;

private:
    T& m_variable;
    T m_original_value;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
