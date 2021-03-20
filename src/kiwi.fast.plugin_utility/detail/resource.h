#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <memory>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

namespace detail
{
    template<typename T, typename CharType>
    class object_memory
    {
    public:
        class arg_type {};

        T* obtain_resource(arg_type const&) const
        {
            return new T;
        }

        void release_resource(arg_type const&, T* object) const
        {
            delete object;
        }
    };

    template<typename T, typename CharType = char8_t, typename SourceTrait = object_memory<T, CharType>>
    class resource
    {
    public:
        using arg_type = typename SourceTrait::arg_type;

        explicit resource(arg_type const& arg_ = arg_type{})
            : m_arg(arg_)
            , m_object(static_cast<T*>(SourceTrait().obtain_resource(arg_)), [arg_](T* object) { SourceTrait().release_resource(arg_, object); })
        {}

        virtual ~resource()
        {}

        arg_type const& get_arg() const
        {
            return m_arg;
        }

        arg_type& get_arg()
        {
            return m_arg;
        }

        T const* get() const
        {
            return m_object.get();
        }

        T* get()
        {
            return m_object.get();
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
        std::shared_ptr<T> m_object;
    };

    ////这种实现出来的是不可拷贝的，不可拷贝的使用起来很不方便
    //template<typename T, typename CharType = char8_t, typename SourceTrait = object_memory<T, CharType>>
    //class resource
    //{
    //protected:
    //    using arg = typename SourceTrait::arg;
    //
    //public:
    //    explicit resource(arg const& arg_ = arg{})
    //        : m_arg(arg_)
    //        , m_own(true)
    //        , m_object(static_cast<T*>(SourceTrait().obtain_resource(arg_)))
    //    {}
    //
    //    explicit resource(T* object, arg const& arg_ = arg{})
    //        : m_arg(arg_)
    //        , m_own(true)
    //        , m_object(object)
    //    {}
    //
    //    resource(resource<T, SourceTrait> const& source) = delete;
    //    resource& operator=(resource<T, SourceTrait> const& source) = delete;
    //
    //    resource(resource<T, SourceTrait>&& source)
    //        :m_arg(source.m_arg)
    //        , m_own(source.m_own)
    //        , m_object(const_cast<T*>(source.release()))
    //    {}
    //
    //    resource& operator=(resource<T, SourceTrait>&& source)
    //    {
    //        if(&source == this)
    //        {
    //            return *this;
    //        }
    //
    //        m_arg = source.m_arg;
    //        if(m_object != source.get())
    //        {
    //            destroy_if_owned();
    //            m_own = source.m_own;
    //        }
    //        else if(!m_own)
    //        {
    //            m_own = source.m_own;
    //        }
    //        m_object = const_cast<T*>(source.release());
    //    }
    //
    //    virtual ~resource()
    //    {
    //        destroy_if_owned();
    //    }
    //
    //    arg const& get_arg() const
    //    {
    //        return m_arg;
    //    }
    //
    //    arg& get_arg()
    //    {
    //        return m_arg;
    //    }
    //
    //    T const* get() const
    //    {
    //        return m_object;
    //    }
    //
    //    T* get()
    //    {
    //        return m_object;
    //    }
    //
    //    T const* operator->() const
    //    {
    //        return get();
    //    }
    //
    //    T* operator->()
    //    {
    //        return get();
    //    }
    //
    //    T const& operator*() const
    //    {
    //        return *get();
    //    }
    //
    //    T& operator*()
    //    {
    //        return *get();
    //    }
    //
    //    T const& operator[](int index) const
    //    {
    //        return get()[index];
    //    }
    //
    //    T& operator[](int index)
    //    {
    //        return get()[index];
    //    }
    //
    //    T const* release() const
    //    {
    //        m_own = false;
    //        return get();
    //    }
    //
    //    T* release()
    //    {
    //        m_own = false;
    //        return get();
    //    }
    //
    //protected:
    //    void destroy_if_owned()
    //    {
    //        if(m_object != nullptr && m_own)
    //        {
    //            SourceTrait().release_resource(m_arg, m_object);
    //            m_object = nullptr;
    //            m_own = false;
    //        }
    //    }
    //
    //private:
    //    arg m_arg;
    //    mutable bool m_own;
    //    T* m_object;
    //};
}

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE
