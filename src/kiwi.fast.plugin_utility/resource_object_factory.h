#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/detail/resource.h>

#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/service_object_factory.h>

#include <string>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

template<typename CharType>
class allocator_object_factory
{
public:
    class arg_type
    {
    public:
        std::basic_string<CharType> m_type;
        arg_type(std::basic_string<CharType> const& type)
            : m_type(type)
        {}
        arg_type(CharType const* type)
            : m_type(type)
        {}
    };

protected:
    service_object_factory* object_factory_service() const
    {
        service<service_object_factory> object_factory_service_;
        return object_factory_service_.get();
    }
};

template<typename T, typename CharType>
class object_factory : public allocator_object_factory<CharType>
{
public:
    using arg_type = typename allocator_object_factory<CharType>::arg_type;

    T* obtain_resource(arg_type const& arg) const
    {
        return std::any_cast<T*>(object_factory::object_factory_service()->create_object(arg.m_type));
    }

    void release_resource(arg_type const& arg, T* object) const
    {
        std::any any_ = object;
        object_factory::object_factory_service()->destroy_object(any_, arg.m_type);
    }
};

template<typename T, typename CharType>
class deque_factory : public allocator_object_factory<CharType>
{
public:
    using arg_type = typename allocator_object_factory<CharType>::arg_type;

    T* obtain_resource(arg_type const& arg) const
    {
        return std::any_cast<T*>(deque_factory::object_factory_service()->create_deque(arg.m_type));
    }

    void release_resource(arg_type const& arg, T* deque) const
    {
        std::any any_ = deque;
        deque_factory::object_factory_service()->destroy_deque(any_, arg.m_type);
    }
};

template<typename T, typename CharType = char8_t>
class resource_object_factory : public detail::resource<T, CharType, object_factory<T, CharType>>
{
public:
    explicit resource_object_factory()
        : detail::resource<T, CharType, object_factory<T, CharType>>(type_converter::to_string<T>())
    {}

    explicit resource_object_factory(T* object)
        : detail::resource<T, CharType, object_factory<T, CharType>>(object, typename detail::resource<T, CharType, object_factory<T, CharType>>::arg(type_converter::to_string<T>()))
    {}
};

template<typename T, typename CharType = char8_t>
class resource_deque_factory : public detail::resource<std::deque<T*>, CharType, deque_factory<std::deque<T*>, CharType>>
{
public:
    explicit resource_deque_factory()
        : detail::resource<std::deque<T*>, CharType, deque_factory<std::deque<T*>, CharType>>(type_converter::to_string<std::deque<T*>>())
    {}

    explicit resource_deque_factory(T* object)
        : detail::resource<std::deque<T*>, CharType, deque_factory<std::deque<T*>, CharType>>(object, typename detail::resource<std::deque<T*>, CharType, deque_factory<std::deque<T*>, CharType>>::arg_type(type_converter::to_string<std::deque<T*>>()))
    {}
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE