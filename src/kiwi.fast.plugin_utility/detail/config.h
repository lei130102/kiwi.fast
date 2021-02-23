#pragma once

#include <interface/config.h>

#define KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE namespace kiwi_fast { inline namespace KIWI_FAST_VERSION_NAMESPACE { namespace plugin_utility {
#define KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE }}}

#define KIWI_FAST_PLUGIN_UTILITY_NAMESPACE kiwi_fast::KIWI_FAST_VERSION_NAMESPACE::plugin_utility
#define KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER kiwi_fast::KIWI_FAST_VERSION_NAMESPACE::plugin_utility::

#define KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_USE using namespace KIWI_FAST_PLUGIN_UTILITY_NAMESPACE;
