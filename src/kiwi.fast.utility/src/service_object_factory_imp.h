#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/type_converter.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <functional>
#include <map>
#include <deque>
#include <any>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_object_factory_imp
{
public:
    //使用std::any而不是void*，可以保存类型信息，std::any保存的是指针
    using create_object_function_type = std::function<std::any()>;
    using destroy_object_function_type = std::function<void(std::any&)>;

    using to_u8string_function_type = std::function<std::u8string(std::any const&)>;
    using from_u8string_function_type = std::function<void(std::any&, std::u8string const& str)>;

    using u8string_create_object_function_map_type = std::map<std::u8string, create_object_function_type>;
    using u8string_create_deque_function_map_type = std::map<std::u8string, create_object_function_type>;
    using u8string_destroy_object_function_map_type = std::map<std::u8string, destroy_object_function_type>;
    using u8string_destroy_deque_function_map_type = std::map<std::u8string, destroy_object_function_type>;

    using u8string_object_u8string_function_map_type = std::map<std::u8string, std::pair<to_u8string_function_type, from_u8string_function_type>>;
    using u8string_deque_u8string_function_map_type = std::map<std::u8string, std::pair<to_u8string_function_type, from_u8string_function_type>>;

public:
    virtual std::any create_object(const char8_t* class_name)
    {
        std::u8string deque_element_class_name;
        if(parse_class_name(class_name, deque_element_class_name))
        {
            return create_object_deque(deque_element_class_name.c_str());
        }
        else
        {
            auto iter = m_create_object_map.find(class_name);
            if(iter != m_create_object_map.end())
            {
                return ((*iter).second)();
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过service_object_factory_imp::create_object创建对象：") + class_name);
            }
        }
    }
    virtual void destroy_object(std::any& object, const char8_t* class_name)
    {
        if(!object.has_value())
        {
            return;
        }

        std::u8string deque_element_class_name;
        if(parse_class_name(class_name, deque_element_class_name))
        {
            destroy_object_deque(object, deque_element_class_name.c_str());
        }
        else
        {
            auto iter = m_destroy_object_map.find(class_name);
            if(iter != m_destroy_object_map.end())
            {
                ((*iter).second)(object);
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过service_object_factory_imp::destroy_object销毁对象：") + class_name);
            }
        }
    }
    virtual std::any create_object_deque(const char8_t* class_name)
    {
        auto iter = m_create_deque_map.find(class_name);
        if(iter != m_create_deque_map.end())
        {
            return ((*iter).second)();
        }
        else
        {
            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过service_object_factory_imp::create_object_deque创建对象：") + class_name);
        }
    }
    virtual void destroy_object_deque(std::any& deque, const char8_t* class_name)
    {
        if(!deque.has_value())
        {
            return;
        }

        auto iter = m_destroy_deque_map.find(class_name);
        if(iter != m_destroy_deque_map.end())
        {
            ((*iter).second)(deque);
        }
        else
        {
            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"无法通过service_object_factory_imp::destroy_object_deque销毁对象：") + class_name);
        }
    }

    virtual std::u8string object_to_string(std::any const& object, const char8_t* class_name)
    {
        std::u8string deque_element_class_name;
        if(parse_class_name(class_name, deque_element_class_name))
        {
            return deque_to_string(object, deque_element_class_name.c_str());
        }
        else
        {
            auto iter = m_object_u8string_map.find(class_name);
            if(iter != m_object_u8string_map.end())
            {
                return ((*iter).second.first)(object);
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"service_object_factory_imp::object_to_string 失败：") + class_name);
            }
        }
    }

    virtual void string_to_object(const char8_t* str, std::any& object, const char8_t* class_name)
    {
        std::u8string deque_element_class_name;
        if(parse_class_name(class_name, deque_element_class_name))
        {
            return string_to_deque(str, object, deque_element_class_name.c_str());
        }
        else
        {
            auto iter = m_object_u8string_map.find(class_name);
            if(iter != m_object_u8string_map.end())
            {
                return ((*iter).second.second)(object, str);
            }
            else
            {
                KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"service_object_factory_imp::string_to_object 失败：") + class_name);
            }
        }
    }

    virtual std::u8string deque_to_string(std::any const& object, const char8_t* class_name)
    {
        auto iter = m_deque_u8string_map.find(class_name);
        if(iter != m_deque_u8string_map.end())
        {
            return ((*iter).second.first)(object);
        }
        else
        {
            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"service_object_factory_imp::deque_to_string 失败：") + class_name);
        }
    }

    virtual void string_to_deque(const char8_t* str, std::any& object, const char8_t* class_name)
    {
        auto iter = m_deque_u8string_map.find(class_name);
        if(iter != m_deque_u8string_map.end())
        {
            ((*iter).second.second)(object, str);
        }
        else
        {
            KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER logic_error, std::u8string(u8"service_object_factory_imp::string_to_deque 失败：") + class_name);
        }
    }

protected:
    template<typename T, typename TImp = T>
    void insert_object_create_destroy()
    {
        service_object_factory_imp::m_create_object_map.insert(u8string_create_object_function_map_type::value_type(
                                                                   KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
                                                                   , [](){return new TImp;}
        ));
        service_object_factory_imp::m_destroy_object_map.insert(u8string_destroy_object_function_map_type::value_type(
                                                                    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
                                                                    , [](std::any& object){delete std::any_cast<TImp*>(object); object.reset();}
        ));
    };
    template<typename T, typename TImp = T>
    void insert_deque_create_destroy()
    {
        service_object_factory_imp::m_create_deque_map.insert(u8string_create_object_function_map_type::value_type(
                                                                   KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
                                                                   , [](){return new std::deque<TImp>;}
        ));
        service_object_factory_imp::m_destroy_deque_map.insert(u8string_destroy_object_function_map_type::value_type(
                                                                    KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
                                                                    , [](std::any& deque){delete std::any_cast<std::deque<TImp>*>(deque); deque.reset();}
        ));
    };

    template<typename T, typename TImp, typename FToString, typename FFromString>
    void insert_object_u8string(FToString f_to_string, FFromString f_from_string)
    {
        service_object_factory_imp::m_object_u8string_map.insert(u8string_object_u8string_function_map_type::value_type(
                                                                        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
                                                                        ,
                                                                        std::make_pair([f = f_to_string](std::any const& object){return f(std::any_cast<TImp*>(object));}
                                                                            , [f = f_from_string](std::any& object, std::u8string const& str){ f(std::any_cast<TImp*>(object), str);})
        ));
    }

    template<typename T, typename TImp, typename FToString, typename FFromString>
    void insert_deque_u8string(FToString f_to_string, FFromString f_from_string)
    {
        service_object_factory_imp::m_deque_u8string_map.insert(u8string_deque_u8string_function_map_type::value_type(
                                                                        KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER type_converter::to_string<T>()
                                                                          ,
                                                                          std::make_pair([f = f_to_string](std::any const& deque){return f(std::any_cast<std::deque<TImp>*>(deque));}
                                                                                , [f = f_from_string](std::any& deque, std::u8string const& str){ f(std::any_cast<std::deque<TImp>*>(deque),str);})
        ));
    }

    service_object_factory_imp();

    virtual ~service_object_factory_imp()
    {}

    bool parse_class_name(const char8_t* class_name, std::u8string& deque_element_class_name)
    {
        static std::u8string const deque_prefix = u8"std::deque<";
        static int const deque_prefix_length = deque_prefix.length();

        std::u8string class_name_ = class_name;
        std::size_t class_name_length = class_name_.length();
        if((class_name_.substr(0, deque_prefix_length) == deque_prefix)
                && (class_name_[class_name_length-1] == u8'>'))
        {
            //比如class_name是std::deque<int>，那么deque_element_class_name就是int
            deque_element_class_name = class_name_.substr(deque_prefix_length, class_name_length - deque_prefix_length - 1);
            return true;
        }
        return false;
    }

private:
    static u8string_create_object_function_map_type m_create_object_map;
    static u8string_destroy_object_function_map_type m_destroy_object_map;

    static u8string_create_deque_function_map_type m_create_deque_map;
    static u8string_destroy_deque_function_map_type m_destroy_deque_map;

    static u8string_object_u8string_function_map_type m_object_u8string_map;
    static u8string_deque_u8string_function_map_type m_deque_u8string_map;
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE

#define SERVICE_OBJECT_FACTORY_ADAPTER_METHOD(imp_class)                                                      \
    std::any create_object(const char8_t* class_name)                                                         \
    {                                                                                                         \
        return imp_class::create_object(class_name);                                                          \
    }                                                                                                         \
    void destroy_object(std::any& object, const char8_t* class_name)                                          \
    {                                                                                                         \
        imp_class::destroy_object(object, class_name);                                                        \
    }                                                                                                         \
    std::any create_object_deque(const char8_t* class_name)                                                   \
    {                                                                                                         \
        return imp_class::create_object_deque(class_name);                                                    \
    }                                                                                                         \
    void destroy_object_deque(std::any& deque, const char8_t* class_name)                                     \
    {                                                                                                         \
        imp_class::destroy_object_deque(deque, class_name);                                                   \
    }                                                                                                         \
    std::u8string object_to_string(std::any const& object, const char8_t* class_name)                         \
    {                                                                                                         \
        return imp_class::object_to_string(object, class_name);                                               \
    }                                                                                                         \
    void string_to_object(const char8_t* str, std::any& object, const char8_t* class_name)                    \
    {                                                                                                         \
        imp_class::string_to_object(str, object, class_name);                                                 \
    }                                                                                                         \
    std::u8string deque_to_string(std::any const& object, const char8_t* class_name)                          \
    {                                                                                                         \
        return imp_class::deque_to_string(object, class_name);                                                \
    }                                                                                                         \
    void string_to_deque(const char8_t* str, std::any& object, const char8_t* class_name)                     \
    {                                                                                                         \
        imp_class::string_to_deque(str, object, class_name);                                                  \
    }
