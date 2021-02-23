#pragma once

#include <interface/config.h>

#define KIWI_FAST_OPEN_MODEL_NAMESPACE namespace kiwi_fast { inline namespace KIWI_FAST_VERSION_NAMESPACE { namespace model {
#define KIWI_FAST_CLOSE_MODEL_NAMESPACE }}}

#define KIWI_FAST_MODEL_NAMESPACE kiwi_fast::KIWI_FAST_VERSION_NAMESPACE::model
#define KIWI_FAST_MODEL_NAMESPACE_QUALIFIER kiwi_fast::KIWI_FAST_VERSION_NAMESPACE::model::

#define KIWI_FAST_MODEL_NAMESPACE_USE using namespace KIWI_FAST_MODEL_NAMESPACE;
