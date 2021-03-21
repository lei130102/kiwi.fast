#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

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
        if (parse_class_name(class_name, class_name_))
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
        from_u8string_function_type f = find_function_by_class_name<from_u8string_function_type>
            (class_name, m_object_from_u8string_map, m_deque_from_u8string_map);
        if (f)
        {
            f(str, object);
        }
    }

protected:
    template<typename T, typename TImp = T>
    void insert_object_create_destroy()
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
    void insert_deque_create_destroy()
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
    void insert_object_u8string(FToString f_to_string, FFromString f_from_string)
	{
		service_object_factory_imp::m_object_to_u8string_map.insert(to_u8string_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			,
			[f = f_to_string](std::any const& object) {return f(std::any_cast<T const*>(object)); }
		));

		service_object_factory_imp::m_object_from_u8string_map.insert(from_u8string_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			,
		    [f = f_from_string](std::u8string const& str, std::any& object) { f(str, std::any_cast<T*>(object)); }
		));
	}

    template<typename T, typename FToString, typename FFromString>
    void insert_deque_u8string(FToString f_to_string, FFromString f_from_string)
	{
		service_object_factory_imp::m_deque_to_u8string_map.insert(to_u8string_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			,
			[f = f_to_string](std::any const& deque) {return f(std::any_cast<std::deque<T*> const*>(deque)); }
		));
		service_object_factory_imp::m_deque_from_u8string_map.insert(from_u8string_function_map_type::value_type(
			KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
			,
		    [f = f_from_string](std::u8string const& str, std::any& deque) { f(str, std::any_cast<std::deque<T*>*>(deque)); }
		));
	}

    service_object_factory_imp();

    virtual ~service_object_factory_imp()
    {}

    bool parse_class_name(const char8_t* class_name, std::u8string& deque_element_class_name)
    {
        static std::u8string const deque_prefix = u8"std::deque<";
        static std::size_t const deque_prefix_length = deque_prefix.length();

        std::u8string class_name_ = class_name;
        std::size_t class_name_length = class_name_.length();
        if((class_name_.substr(0, deque_prefix_length) == deque_prefix)
                && (class_name_[class_name_length-1] == u8'>'))
        {
            //比如class_name是std::deque<int*>，那么deque_element_class_name就是int
            deque_element_class_name = class_name_.substr(deque_prefix_length, class_name_length - deque_prefix_length - 1 - 1);
            return true;
        }
        return false;
    }

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
    }                                                                                                                                                      
