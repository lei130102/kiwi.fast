#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/resource.h>

#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/service_object_factory.h>

#include <string>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

template<typename CharType>
class allocator_object_factory
{
public:
    class arg
    {
    public:
        std::basic_string<CharType> m_type;
        arg(std::basic_string<CharType> const& type)
            : m_type(type)
        {}
        arg(CharType const* type)
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
    using arg = typename allocator_object_factory<CharType>::arg;

    T* obtain_resource(arg const& arg_) const
    {
        return std::any_cast<T*>(object_factory::object_factory_service()->create_object(arg_.m_type));
    }

    void release_resource(arg const& arg_, T* object) const
    {
        std::any any_ = object;
        object_factory::object_factory_service()->destroy_object(any_, arg_.m_type);
    }
};

template<typename T, typename CharType>
class deque_factory : public allocator_object_factory<CharType>
{
public:
    using arg = typename allocator_object_factory<CharType>::arg;

    T* obtain_resource(arg const& arg_) const
    {
        return std::any_cast<T*>(deque_factory::object_factory_service()->create_object_deque(arg_.m_type));
    }

    void release_resource(arg const& arg_, T* deque_) const
    {
        std::any any_ = deque_;
        deque_factory::object_factory_service()->destroy_object_deque(any_, arg_.m_type);
    }
};

template<typename T, typename CharType = char8_t>
class resource_object_factory : public resource<T, CharType, object_factory<T, CharType>>
{
public:
    explicit resource_object_factory()
        : resource<T, CharType, object_factory<T, CharType>>(type_converter::to_string<T>())
    {}

    explicit resource_object_factory(T* object)
        : resource<T, CharType, object_factory<T, CharType>>(object, typename resource<T, CharType, object_factory<T, CharType>>::arg(type_converter::to_string<T>()))
    {}
};

template<typename T, typename CharType = char8_t>
class resource_deque_factory : public resource<T, CharType, deque_factory<T, CharType>>
{
public:
    explicit resource_deque_factory()
        : resource<T, CharType, deque_factory<T, CharType>>(type_converter::to_string<T>())
    {}

    explicit resource_deque_factory(T* object)
        : resource<T, CharType, deque_factory<T, CharType>>(object, typename resource<T, CharType, deque_factory<T, CharType>>::arg(type_converter::to_string<T>()))
    {}
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
