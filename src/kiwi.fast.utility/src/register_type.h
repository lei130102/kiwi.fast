#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/exceptions.h>

#include <string>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class register_type
{
public:
	static register_type* create()
	{
		if (!m_instance)
		{
			if (m_destroyed)
			{
				KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"register_type 已被销毁");
				return nullptr;
			}
			m_instance = new register_type;
			m_destroyed = false;
		}

		return m_instance;
	}

	static void destroy()
	{
		if (m_destroyed)
		{
			KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"register_type 已被销毁");
			return;
		}
		delete m_instance;
		m_instance = nullptr;
		m_destroyed = true;
	}

	static bool parse_class_name(const char8_t* class_name, std::u8string& deque_element_class_name)
	{
		static std::u8string const deque_prefix = u8"std::deque<";
		static std::size_t const deque_prefix_length = deque_prefix.length();

		std::u8string class_name_ = class_name;
		std::size_t class_name_length = class_name_.length();
		if ((class_name_.substr(0, deque_prefix_length) == deque_prefix)
			&& (class_name_[class_name_length - 1] == u8'>'))
		{
			//比如class_name是std::deque<int*>，那么deque_element_class_name就是int
			deque_element_class_name = class_name_.substr(deque_prefix_length, class_name_length - deque_prefix_length - 1 - 1);
			return true;
		}
		return false;
	}

	static std::u8string separation()
	{
		return m_separation;
	}

private:
	register_type();

	~register_type();

	class deleter
	{
	public:
		~deleter()
		{
			register_type::destroy();
		}
	};

	static deleter m_deleter;

private:
	static register_type* m_instance;
	static bool m_destroyed;

	static std::u8string m_separation;//分隔符号
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE