#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

namespace detail
{
    namespace type_traits
    {
        template<typename T>
        struct is_character_type
        {
            static constexpr bool value = false;
        };

        template<>
        struct is_character_type<char>
        {
            static constexpr bool value = true;
        };

        template<>
        struct is_character_type<wchar_t>
        {
            static constexpr bool value = true;
        };

        template<>
        struct is_character_type<char8_t>
        {
            static constexpr bool value = true;
        };

        template<typename T>
        inline constexpr bool is_character_type_v = is_character_type<T>::value;
    }
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
