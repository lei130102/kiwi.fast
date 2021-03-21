#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <typeindex>
#include <any>
#include <optional>
#include <string>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

namespace detail
{
	class data_value_wrapper
	{
	public:
		virtual ~data_value_wrapper() {};

		virtual std::type_index type_index() const = 0;
		virtual std::optional<std::u8string> inside_type() const = 0;
		virtual data_value_wrapper* copy() const = 0;
		virtual bool operator==(data_value_wrapper const& rhs) const = 0;
		virtual void* pointer() = 0;
		virtual void const* pointer() const = 0;
		virtual void* release_object() = 0;
	};

}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE