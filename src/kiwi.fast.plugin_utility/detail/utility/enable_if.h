#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

namespace detail
{
    namespace utility
    {
        template<typename T, typename R = void>
        struct enable_if_has_type
        {
            using type = R;
        };

        template<bool B, typename T = void>
        struct enable_if_c
        {
            using type = T;
        };
        template<typename T>
        struct enable_if_c<false, T>
        {
            //without type
        };
        template<bool B, typename T = void>
        using enable_if_c_t = typename enable_if_c<B, T>::type;

        template<typename Cond, typename T = void>
        struct enable_if : enable_if_c<Cond::value, T>{};

        template<typename Cond, typename T = void>
        using enable_if_t = typename enable_if<Cond, T>::type;

        template<bool B, typename T>
        struct lazy_enable_if_c
        {
            using type = typename T::type;
        };
        template<typename T>
        struct lazy_enable_if_c<false, T>
        {
            //without type
        };

        template<typename Cond, typename T>
        struct lazy_enable_if : lazy_enable_if_c<Cond::value, T>{};

        template<typename Cond, typename T>
        using lazy_enable_if_t = typename lazy_enable_if<Cond, T>::type;

        template<bool B, typename T = void>
        struct disable_if_c
        {
            using type = T;
        };

        template<typename T>
        struct disable_if_c<true, T>
        {
            //without type
        };

        template<typename Cond, typename T>
        struct disable_if : disable_if_c<Cond::value, T>{};

        template<typename Cond, typename T>
        using disable_if_t = typename disable_if<Cond, T>::type;

        template<bool B, typename T>
        struct lazy_disable_if_c
        {
            using type = typename T::type;
        };
        template<typename T>
        struct lazy_disable_if_c<true, T>
        {
            //without type
        };

        template<typename Cond, typename T>
        struct lazy_disable_if : lazy_disable_if_c<Cond::value, T>{};

        template<typename Cond, typename T>
        using lazy_disable_if_t = typename lazy_disable_if<Cond, T>::type;
    }
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
