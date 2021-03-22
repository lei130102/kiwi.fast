#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/exceptions.h>
#include <kiwi.fast.plugin_utility/service.h>
#include <kiwi.fast.plugin_utility/service_object_factory.h>

#include <kiwi.fast.utility/src/register_type.h>

#include <functional>
#include <map>
#include <deque>
#include <any>
#include <optional>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_object_factory_imp
{
public:

    //

    //使用std::any而不是void*，可以保存类型信息，std::any保存的是指针
    using create_function_type = std::function<std::any()>;
    using copy_function_type = std::function<std::any(std::any const&)>;
    using destroy_function_type = std::function<void(std::any&)>;

    using u8string_create_function_map_type = std::map<std::u8string, create_function_type>;
    using u8string_copy_function_map_type = std::map<std::u8string, copy_function_type>;
    using u8string_destroy_function_map_type = std::map<std::u8string, destroy_function_type>;

    //

    using to_u8string_function_type = std::function<std::optional<std::u8string>(std::any const&)>;
    using from_u8string_function_type = std::function<void(const char8_t* str, std::any&)>;

    using to_u8string_function_map_type = std::map<std::u8string, to_u8string_function_type>;
    using from_u8string_function_map_type = std::map<std::u8string, from_u8string_function_type>;

public:

    template<typename FunctionType, typename ObjectMap, typename DequeMap>
    FunctionType find_function_by_class_name(const char8_t* class_name, ObjectMap object_map, DequeMap deque_map)
    {
        std::u8string class_name_ = class_name;
        if (register_type::parse_class_name(class_name, class_name_))
        {
            auto iter = deque_map.find(class_name_);
            if (iter != deque_map.end())
            {
                return (*iter).second;
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法根据类型名找到对应函数，类型名：") + class_name_);
                return FunctionType{};
            }
        }
        else
        {
            auto iter = object_map.find(class_name_);
            if (iter != object_map.end())
            {
                return (*iter).second;
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法根据类型名找到对应函数，类型名：") + class_name_);
                return FunctionType{};
            }
        }
    }

    virtual std::any create_object(const char8_t* class_name)
    {
        create_function_type f = find_function_by_class_name<create_function_type>
            (class_name, m_create_object_map, m_create_deque_map);
        if (f)
        {
            return f();
        }
        else
        {
            return std::any{};
        }
    }

    virtual std::any copy_object(const char8_t* class_name, std::any const& rhs)
    {
        copy_function_type f = find_function_by_class_name<copy_function_type>
            (class_name, m_copy_object_map, m_copy_deque_map);
        if (f)
        {
            return f(rhs);
        }
        else
        {
            return std::any{};
        }
    }

    virtual void destroy_object(std::any& object, const char8_t* class_name)
    {
        if(!object.has_value())
        {
            return;
        }

        destroy_function_type f = find_function_by_class_name<destroy_function_type>
            (class_name, m_destroy_object_map, m_destroy_deque_map);
        if (f)
        {
            f(object);
        }
    }

    virtual std::optional<std::u8string> object_to_string(std::any const& object, const char8_t* class_name)
    {
        to_u8string_function_type f = find_function_by_class_name<to_u8string_function_type>
            (class_name, m_object_to_u8string_map, m_deque_to_u8string_map);
        if (f)
        {
            return f(object);
        }
        else
        {
            return {};
        }
    }

    virtual void object_from_string(const char8_t* str, const char8_t* class_name, std::any& object)
    {
        if (object.has_value())
        {
            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"object 不能含有值");
        }
        from_u8string_function_type f = find_function_by_class_name<from_u8string_function_type>
            (class_name, m_object_from_u8string_map, m_deque_from_u8string_map);
        if (f)
        {
            f(str, object);
        }
    }

    template<typename T, typename TImp = T>
    static void insert_object_create_destroy()
	{
		service_object_factory_imp::m_create_object_map.insert(u8string_create_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			, []() {return new TImp; }
		));
        service_object_factory_imp::m_copy_object_map.insert(u8string_copy_function_map_type::value_type(
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
            , [](std::any const& object) {
                if (object.type() == typeid(T*))
                {
                    return new TImp(*static_cast<TImp*>(std::any_cast<T*>(object))); 
                }
                else if (object.type() == typeid(T const*))
                {
                    return new TImp(*static_cast<TImp const*>(std::any_cast<T const*>(object))); 
                }
                else
                {
                    KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"copy_object函数的any参数中只能存放T*或者T const*类型");
                    return static_cast<TImp*>(nullptr);
                }
            }
        ));
		service_object_factory_imp::m_destroy_object_map.insert(u8string_destroy_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			, [](std::any& object) {
                if (object.type() == typeid(T*))
                {
                    delete static_cast<TImp*>(std::any_cast<T*>(object)); object.reset();
                }
                else if (object.type() == typeid(T const*))
                {
                    delete static_cast<TImp const*>(std::any_cast<T const*>(object)); object.reset();
                }
                else
                {
                    KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"destroy_object函数的any参数中只能存放T*或者T const*类型");
                }
            }
		));
	};
    template<typename T, typename TImp = T>
    static void insert_deque_create_destroy()
	{
		service_object_factory_imp::m_create_deque_map.insert(u8string_create_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			, []() {
                return new std::deque<T*>; 
            }
		));
        service_object_factory_imp::m_copy_deque_map.insert(u8string_copy_function_map_type::value_type(
            KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
            , [](std::any const& deque) {
                auto ret = new std::deque<T*>;

                std::deque<T*> const* deque_ = nullptr;

                if (deque.type() == typeid(std::deque<T*>*))
                {
                    deque_ = std::any_cast<std::deque<T*>*>(deque);
                }
                else if (deque.type() == typeid(std::deque<T*> const*))
                {
                    deque_ = std::any_cast<std::deque<T*> const*>(deque);
                }
                else
                {
                    KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"copy_object函数的any参数中只能存放T*或者T const*类型");
                    return ret;
                }

			    std::u8string class_name = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>();
                std::for_each(deque_->begin(), deque_->end(), [ret, &class_name](T const* pt) {
                    auto iter = m_copy_object_map.find(class_name);
                    if (iter != m_copy_object_map.end())
                    {
                        ret->push_back(std::any_cast<T*>(((*iter).second)(std::any(pt))));
                    }
                    else
                    {
                        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"没有找到类型名对应函数，类型名：") + class_name);
                    }
                });
                return ret;
            }
        ));
		service_object_factory_imp::m_destroy_deque_map.insert(u8string_destroy_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			, [](std::any& deque) {

                std::deque<T*> const* deque_ = nullptr;

                if (deque.type() == typeid(std::deque<T*>*))
                {
                    deque_ = std::any_cast<std::deque<T*>*>(deque);
                }
                else if (deque.type() == typeid(std::deque<T*> const*))
                {
                    deque_ = std::any_cast<std::deque<T*> const*>(deque);
                }
                else
                {
                    KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"destroy_object函数的any参数中只能存放T*或者T const*类型");
                }
				std::for_each(deque_->begin(), deque_->end(), [](T* pt) { delete static_cast<TImp*>(pt); });
				delete deque_; deque.reset(); }
		));
	};

    template<typename T, typename FToString, typename FFromString>
    static void insert_object_u8string(FToString f_to_string, FFromString f_from_string)
	{
		service_object_factory_imp::m_object_to_u8string_map.insert(to_u8string_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			,
			[f = f_to_string](std::any const& object) {

                if (object.type() == typeid(T*))
                {
                    return f(std::any_cast<T*>(object));
                }
                else if (object.type() == typeid(T const*))
                {
                    return f(std::any_cast<T const*>(object));
                }
                else
                {
                    KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"object_to_string函数的any参数中只能存放T*或者T const*类型");
                    return f(nullptr);
                }
            }
		));

		service_object_factory_imp::m_object_from_u8string_map.insert(from_u8string_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			,
		    [f = f_from_string](std::u8string const& str, std::any& object) { 
                //注意，这里要求传入的object为空，因为这里没有销毁
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                object = std::any_cast<T*>(object_factory_service->create_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()));
                f(str, std::any_cast<T*>(object));
            }
		));
	}

    template<typename T, typename FToString, typename FFromString>
    static void insert_deque_u8string(FToString f_to_string, FFromString f_from_string)
	{
		service_object_factory_imp::m_deque_to_u8string_map.insert(to_u8string_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			,
			[f = f_to_string](std::any const& deque) {

                if (deque.type() == typeid(std::deque<T*>*))
                {
                    return f(std::any_cast<std::deque<T*>*>(deque)); 
                }
                else if (deque.type() == typeid(std::deque<T*> const*))
                {
                    return f(std::any_cast<std::deque<T*> const*>(deque)); 
                }
                else
                {
                    KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, u8"object_to_string函数的any参数中只能存放T*或者T const*类型");
                    return f(nullptr);
                }
            }
		));
		service_object_factory_imp::m_deque_from_u8string_map.insert(from_u8string_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			,
		    [f = f_from_string](std::u8string const& str, std::any& deque) {

                //注意，这里要求传入的object为空，因为这里没有销毁
                KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service<KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER service_object_factory> object_factory_service;
                deque = std::any_cast<std::deque<T*>*>(object_factory_service->create_object(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<std::deque<T*>>()));
                f(str, std::any_cast<std::deque<T*>*>(deque));
            }
		));
	}

protected:
    service_object_factory_imp()
        :m_register_type(register_type::create())
    {}

    virtual ~service_object_factory_imp()
    {}

private:
    register_type* m_register_type;

private:
    static u8string_create_function_map_type m_create_object_map;
    static u8string_copy_function_map_type m_copy_object_map;
    static u8string_destroy_function_map_type m_destroy_object_map;

    static u8string_create_function_map_type m_create_deque_map;
    static u8string_copy_function_map_type m_copy_deque_map;
    static u8string_destroy_function_map_type m_destroy_deque_map;

    //

    static to_u8string_function_map_type m_object_to_u8string_map;
    static from_u8string_function_map_type m_object_from_u8string_map;

    static to_u8string_function_map_type m_deque_to_u8string_map;
    static from_u8string_function_map_type m_deque_from_u8string_map;
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE

#define SERVICE_OBJECT_FACTORY_ADAPTER_METHOD(imp_class)                                                                                                   \
    std::any create_object(const char8_t* class_name)                                                                                                      \
    {                                                                                                                                                      \
        return imp_class::create_object(class_name);                                                                                                       \
    }                                                                                                                                                      \
    std::any copy_object(const char8_t* class_name, std::any const& rhs)                                                                                   \
    {                                                                                                                                                      \
        return imp_class::copy_object(class_name, rhs);                                                                                                    \
    }                                                                                                                                                      \
    void destroy_object(std::any& object, const char8_t* class_name)                                                                                       \
    {                                                                                                                                                      \
        imp_class::destroy_object(object, class_name);                                                                                                     \
    }                                                                                                                                                      \
    std::optional<std::u8string> object_to_string(std::any const& object, const char8_t* class_name)                                                       \
    {                                                                                                                                                      \
        return imp_class::object_to_string(object, class_name);                                                                                            \
    }                                                                                                                                                      \
    void object_from_string(const char8_t* str, const char8_t* class_name, std::any& object)                                                               \
    {                                                                                                                                                      \
        imp_class::object_from_string(str, class_name, object);                                                                                            \
    }
