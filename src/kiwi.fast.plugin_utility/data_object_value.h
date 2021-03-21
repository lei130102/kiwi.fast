#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/detail/typed_data_value.h>
#include <kiwi.fast.plugin_utility/detail/data_value_wrapper.h>


#include <memory>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

template<typename T, typename CharType = char8_t>
class data_object_value : public detail::data_value_wrapper, public detail::typed_data_value<T, CharType>
{
public:
	data_object_value()
	{}

	data_object_value(T* object)
		: detail::typed_data_value<T, CharType>(object)
	{}

	//注意，这里不能使用默认拷贝构造函数，因为这里要实现深拷贝，但类型T可以是自定义虚基类，也可以是基本类型
	data_object_value(data_object_value<T, CharType> const& rhs)
	{
		this->copy_reset(rhs.get());
	}

	data_object_value& operator=(data_object_value<T, CharType> const& rhs)
	{
		this->copy_reset(rhs.get());
	}

	std::type_index type_index() const override
	{
		return typeid(data_object_value<T, CharType>);
	}

	std::optional<std::u8string> inside_type() const
	{
		return this->arg().m_type;
	}

	data_value_wrapper* copy() const override
	{
		return new data_object_value<T, CharType>(*this);
	}

	bool operator==(data_value_wrapper const& rhs) const override
	{
		if (rhs.type_index() != type_index())
		{
			return false;
		}

		data_object_value<T, CharType> const& tmp = static_cast<data_object_value<T, CharType> const&>(rhs);
		return this->operator*() == *tmp;
	}

	void* pointer() override
	{
		return this->get();
	}

	void const* pointer() const override
	{
		return this->get();
	}

	void* release_object() override
	{
		return this->release();
	}
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
