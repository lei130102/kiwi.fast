#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/type_converter.h>

#include <any>
#include <memory>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

class service_object_factory;

namespace detail
{
    template<typename T, typename CharType>
	class allocator_data_value
	{
    public:
        struct arg_type
        {
            std::basic_string<CharType> m_type;
            
            arg_type(std::basic_string<CharType> const& type)
                :m_type(type)
            {}

            arg_type(CharType const* type)
                :m_type(type)
            {}
        };

        //因为在类模板中，所以可以直接使用仅提供前置声明的service_object_factory的成员函数create_object、copy_object和destroy_object

        T* obtain(arg_type const& arg) const
        {
            return std::any_cast<T*>(factory_service()->create_object(arg.m_type));
        }

        T* copy(T const* object) const
        {
            return std::any_cast<T*>(factory_service()->copy_object(type_converter::to_string<T>(), std::any(object)));
        }

        void release(arg_type const& arg, T* object) const
        {
            std::any any_(object);
            factory_service()->destroy_object(any_, arg.m_type);
        }

    protected:
        service_object_factory* factory_service() const
        {
            service<service_object_factory> factory_service;
            return factory_service.get();
        }
	};


    template<typename T, typename CharType = char8_t, typename SourceTrait = allocator_data_value<T, CharType>>
    class typed_data_value
    {
    public:
        using arg_type = typename SourceTrait::arg_type;

        typed_data_value()
            : m_arg(type_converter::to_string<T>())
            , m_object(SourceTrait().obtain(m_arg))
        {}

        typed_data_value(T* object)
            : m_arg(type_converter::to_string<T>())
            , m_object(nullptr)
        {
            reset(object);
        }

        virtual ~typed_data_value()
        {
            SourceTrait().release(this->m_arg, m_object);
        }

        void reset(T* object)
        {
            SourceTrait().release(this->m_arg, m_object);
            m_object = object;
        }

        void copy_reset(T const* object)
        {
            SourceTrait().release(this->m_arg, m_object);
            m_object = SourceTrait().copy(object);
        }

        arg_type const& arg() const
        {
            return m_arg;
        }

        arg_type& arg()
        {
            return m_arg;
        }

        T* release()
        {
            T* ret = m_object;
            m_object = nullptr;
            return ret;
        }

        T const* get() const
        {
            return m_object;
        }

        T* get()
        {
            return m_object;
        }

        T const* operator->() const
        {
            return get();
        }

        T* operator->()
        {
            return get();
        }

        T const& operator*() const
        {
            return *get();
        }

        T& operator*()
        {
            return *get();
        }

        T const& operator[](int index) const
        {
            return get()[index];
        }

        T& operator[](int index)
        {
            return get()[index];
        }

    private:
        arg_type m_arg;
        T* m_object;    //没必要使用std::shared_ptr，std::unique_ptr
    };
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
