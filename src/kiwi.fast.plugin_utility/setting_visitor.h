#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/service.h>

#include <kiwi.fast.plugin_utility/ptree_item.h>
#include <kiwi.fast.plugin_utility/ptree_visitor.h>

#include <string>
#include <deque>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class setting_visitor : public base_ptree_visitor
{
public:
    setting_visitor(base_ptree_item* ptree_root)
        : base_ptree_visitor(ptree_root)
    {}
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

