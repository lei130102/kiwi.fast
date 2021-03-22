#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/detail/data_value_wrapper.h>
#include <kiwi.fast.plugin_utility/data_object_value.h>
#include <kiwi.fast.plugin_utility/data_deque_value.h>
#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <deque>
#include <optional>
#include <string>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class data_value
{
public:
	data_value()
		: m_data_value_wrapper(nullptr)
	{}

	template<typename CharType>
	static data_value create(CharType const* class_name)
	{
		return create(std::basic_string<CharType>(class_name));
	}
	template<typename CharType>
	static data_value create(std::basic_string<CharType> const& class_name)
	{
		return create(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(class_name).c_str());
	}
	static data_value create(const char8_t* class_name);

	data_value(data_value const& rhs)
		: m_data_value_wrapper(nullptr)
	{
		if (rhs.m_data_value_wrapper)
		{
			m_data_value_wrapper = rhs.m_data_value_wrapper->copy();
		}
	}
	data_value& operator=(data_value const& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		detail::data_value_wrapper* tmp = m_data_value_wrapper;
		if (rhs.m_data_value_wrapper)
		{
			m_data_value_wrapper = rhs.m_data_value_wrapper->copy();
		}
		else
		{
			m_data_value_wrapper = nullptr;
		}
		delete tmp;

		return *this;
	}

	data_value(data_value&& rhs)
		: m_data_value_wrapper(nullptr)
	{
		swap(std::move(rhs));
	}

	data_value& operator=(data_value&& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		swap(std::move(rhs));
	}

	~data_value()
	{
		delete m_data_value_wrapper;
	}

	void swap(data_value&& rhs)
	{
		detail::data_value_wrapper* tmp = m_data_value_wrapper;
		m_data_value_wrapper = rhs.m_data_value_wrapper;
		rhs.m_data_value_wrapper = tmp;
	}

	bool is_valid() const
	{
		return m_data_value_wrapper != nullptr;
	}

	std::optional<std::u8string> inside_type() const
	{
		if (m_data_value_wrapper == nullptr)
		{
			return {};
		}
		return m_data_value_wrapper->inside_type();
	}

	//不提供operator==和operator!=，因为内部类型除了基本类型指针外，还可能是虚基类指针，更还有指针数组的可能，统一接口比较麻烦
	//最好还是获得指针后再通过指针进行比较操作

	std::optional<std::u8string> to_string() const;

	template<typename CharType>
	static data_value from_string(CharType const* str, CharType const* class_name)
	{
		return from_string(std::basic_string<CharType>(str), std::basic_string<CharType>(class_name));
	}
	template<typename CharType>
	static data_value from_string(std::basic_string<CharType> const& str, std::basic_string<CharType> const& class_name)
	{
		return from_string(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<CharType>(str).c_str()
			, KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<CharType>(class_name).c_str());
	}
	static data_value from_string(const char8_t* str, const char8_t* class_name);

public:
	template<typename T>
	explicit data_value(data_object_value<T> const& value)
		: m_data_value_wrapper(new data_object_value<T>(value))
	{}

	template<typename T>
	explicit data_value(data_deque_value<T> const& value)
		: m_data_value_wrapper(new data_deque_value<T>(value))
	{}

	template<typename T>
	data_value& operator=(data_object_value<T> const& value)
	{
		detail::data_value_wrapper* tmp = m_data_value_wrapper;

		m_data_value_wrapper = new data_object_value<T>(value);

		if (tmp)
		{
			delete tmp;
		}
	}

	template<typename T>
	data_value& operator=(data_deque_value<T> const& value)
	{
		detail::data_value_wrapper* tmp = m_data_value_wrapper;

		m_data_value_wrapper = new data_deque_value<T>(value);

		if (tmp)
		{
			delete tmp;
		}
	}

	void* pointer_as_void()
	{
		return const_cast<void*>(static_cast<data_value const&>(*this).pointer_as_void());
	}

	void const* pointer_as_void() const
	{
		if (m_data_value_wrapper)
		{
			return m_data_value_wrapper->pointer();
		}
		else
		{
			return nullptr;
		}
	}

	void* release_as_void()
	{
		if (m_data_value_wrapper)
		{
			void* ret =  m_data_value_wrapper->release_object();
			delete m_data_value_wrapper;
			m_data_value_wrapper = nullptr;
			return ret;
		}
		else
		{
			return nullptr;
		}
	}

	template<typename T>
	T& value()
	{
		return const_cast<T&>(static_cast<data_value const&>(*this).value<T>());
	}

	template<typename T>
	T const& value() const
	{
		void const* p = pointer_as_void();
		if (p == nullptr)
		{
			KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"无效值 data_value 对象，无法获取其值");
		}
		return *static_cast<T const*>(p);
	}

	template<typename T>
	T* pointer()
	{
		return const_cast<T*>(static_cast<data_value const&>(*this).pointer<T>());
	}

	template<typename T>
	T const* pointer() const
	{
		return static_cast<T const*>(pointer_as_void());
	}

	template<typename T>
	T* release()
	{
		return static_cast<T*>(release_as_void());
	}

	template<typename T>
	void reset(data_object_value<T> const& value)
	{
		if (m_data_value_wrapper)
		{
			delete m_data_value_wrapper;
		}
		m_data_value_wrapper = new data_object_value<T>(value);
	}

	template<typename T>
	void reset(data_deque_value<T> const& value)
	{
		if (m_data_value_wrapper)
		{
			delete m_data_value_wrapper;
		}
		m_data_value_wrapper = new data_deque_value<T>(value);
	}

private:
	detail::data_value_wrapper* m_data_value_wrapper;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE