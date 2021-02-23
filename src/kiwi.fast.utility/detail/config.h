#pragma once

#include <interface/config.h>

#define KIWI_FAST_OPEN_UTILITY_NAMESPACE namespace kiwi_fast { inline namespace KIWI_FAST_VERSION_NAMESPACE { namespace utility {
#define KIWI_FAST_CLOSE_UTILITY_NAMESPACE }}}

#define KIWI_FAST_UTILITY_NAMESPACE kiwi_fast::KIWI_FAST_VERSION_NAMESPACE::utility
#define KIWI_FAST_UTILITY_NAMESPACE_QUALIFIER kiwi_fast::KIWI_FAST_VERSION_NAMESPACE::utility::

#define KIWI_FAST_UTILITY_NAMESPACE_USE using namespace KIWI_FAST_UTILITY_NAMESPACE;
