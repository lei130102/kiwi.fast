#pragma once

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/service.h>

#include <kiwi.fast.plugin_utility/ptree_root.h>
#include <kiwi.fast.plugin_utility/ptree_item.h>

#include <string>
#include <deque>

KIWI_FAST_OPEN_PLUGIN_UTILITY_NAMESPACE

template<typename Setting>
class setting_visitor : public ptree_visitor
{
    friend Setting;

public:
    setting_visitor(Setting* setting_service)
        :m_setting_service(setting_service)
    {}

    virtual ~setting_visitor()
    {}

    virtual std::optional<name_type> name()
    {
        if(m_tmp)
        {
            auto ptree_item = find_setting_tmp_object_value_item(m_names);
            if(ptree_item)
            {
                //临时配置
                return (*ptree_item).get().name();
            }
            else
            {
                return {};
            }
        }
        else
        {
            auto ptree_item = find_setting_tmp_object_value_item(m_names);
            if(ptree_item)
            {
                //临时配置
                return (*ptree_item).get().name();
            }
            else
            {
                auto ptree_item = find_setting_user_object_value_item(m_names);
                if(ptree_item)
                {
                    //用户配置
                    return (*ptree_item).get().name();
                }
                else
                {
                    auto ptree_item = find_setting_default_object_value_item(m_names);
                    if(ptree_item)
                    {
                        //缺省配置
                        return (*ptree_item).get().name();
                    }
                    else
                    {
                        return {};
                    }
                }
            }
        }
    }

    std::optional<type_type> type()
    {
        if(m_tmp)
        {
            auto ptree_item = find_setting_tmp_object_value_item(m_names);
            if(ptree_item)
            {
                //临时配置
                return (*ptree_item).get().type();
            }
            else
            {
                return {};
            }
        }
        else
        {
            auto ptree_item = find_setting_tmp_object_value_item(m_names);
            if(ptree_item)
            {
                //临时配置
                return (*ptree_item).get().type();
            }
            else
            {
                auto ptree_item = find_setting_user_object_value_item(m_names);
                if(ptree_item)
                {
                    //用户配置
                    return (*ptree_item).get().type();
                }
                else
                {
                    auto ptree_item = find_setting_default_object_value_item(m_names);
                    if(ptree_item)
                    {
                        //缺省配置
                        return (*ptree_item).get().type();
                    }
                    else
                    {
                        return {};
                    }
                }
            }
        }
    }


    //不知道是否能被继承，暂时注释掉
    //template<typename T>
    //std::optional<resource_object_factory<T>> value()
    //{
    //    auto result = value();
    //    if (result)
    //    {
    //        try {
    //            return std::any_cast<resource_object_factory<T>>(*result);
    //        }
    //        catch (std::bad_any_cast& e) {
    //            return {};
    //        }
    //    }
    //    else
    //    {
    //        return {};
    //    }
    //}

    //template<typename T>
    //std::optional<resource_deque_factory<T>> value()
    //{
    //    auto result = value();
    //    if (result)
    //    {
    //        try {
    //            return std::any_cast<resource_deque_factory<T>>(*result);
    //        }
    //        catch (std::bad_any_cast& e) {
    //            return {};
    //        }
    //    }
    //    else
    //    {
    //        return {};
    //    }
    //}

    //template<typename T>
    //std::optional<resource_object_factory<T>> value(resource_object_factory<T> const& default_value)
    //{
    //    auto result = value();
    //    if (result)
    //    {
    //        try
    //        {
    //            return std::any_cast<resource_object_factory<T>>(*result);
    //        }
    //        catch (std::bad_any_cast& e)
    //        {
    //            return default_value;
    //        }
    //    }
    //    else
    //    {
    //        return default_value;
    //    }
    //}

    //template<typename T>
    //std::optional<resource_deque_factory<T>> value(resource_deque_factory<T> const& default_value)
    //{
    //    auto result = value();
    //    if (result)
    //    {
    //        try
    //        {
    //            return std::any_cast<resource_deque_factory<T>>(*result);
    //        }
    //        catch (std::bad_any_cast& e)
    //        {
    //            return default_value;
    //        }
    //    }
    //    else
    //    {
    //        return default_value;
    //    }
    //}

    std::optional<value_type> value()
    {
        if(m_tmp)
        {
            auto ptree_item = find_setting_tmp_object_value_item(m_names);
            if(ptree_item)
            {
                //临时配置
                return (*ptree_item).get().value();
            }
            else
            {
                return {};
            }
        }
        else
        {
            auto ptree_item = find_setting_tmp_object_value_item(m_names);
            if(ptree_item)
            {
                //临时配置
                return (*ptree_item).get().value();
            }
            else
            {
                auto ptree_item = find_setting_user_object_value_item(m_names);
                if(ptree_item)
                {
                    //用户配置
                    return (*ptree_item).get().value();
                }
                else
                {
                    auto ptree_item = find_setting_default_object_value_item(m_names);
                    if(ptree_item)
                    {
                        //缺省配置
                        return (*ptree_item).get().value();
                    }
                    else
                    {
                        return {};
                    }
                }
            }
        }
    }

    template<typename T>
    bool set_value(T* value)
    {
        if(m_tmp)
        {
            auto ptree_item = find_setting_tmp_object_value_item(m_names);
            if(ptree_item)
            {
                //////销毁之前的
                ////service<service_object_factory> object_factory_service;
                ////object_factory_service->destroy_object((*ptree_item).get().value, (*ptree_item).get().type);

                //设置type和value
                (*ptree_item).get().type() = type_converter::to_string<T>();
                (*ptree_item).get().value() = value;

                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            auto ptree_item = find_setting_user_object_value_item(m_names);
            if(ptree_item)
            {
                ////销毁之前的
                //service<service_object_factory> object_factory_service;
                //object_factory_service->destroy_object((*ptree_item).get().value, (*ptree_item).get().type);

                //设置type和value
                (*ptree_item).get().type() = type_converter::to_string<T>();
                (*ptree_item).get().value() = value;

                return true;
            }
            else
            {
                return false;
            }
        }
    }

protected:

    void add_name(std::u8string const& name)
    {
        m_names.push_back(name);
    }

    void set_tmp(bool is_tmp)
    {
        m_tmp = is_tmp;
    }

    std::optional<std::reference_wrapper<ptree_item>> find_setting_default_object_value_item(std::deque<std::u8string>& names)
    {
        if (names.size() == 0)
        {
            return {};
        }

        if (m_setting_service == nullptr)
        {
            return {};
        }

        for (auto& ptree_item_ : m_setting_service->setting_default().items())
        {
            if (ptree_item_.name() == names.front())
            {
                if (ptree_item_.value().type() == typeid(std::deque<ptree_item>))
                {
                    names.pop_front();
                    return find_setting_default_object_value_item(names);
                }
                else
                {
                    return ptree_item_;
                }
            }
        }
        return {};
    }

    std::optional<std::reference_wrapper<ptree_item>> find_setting_user_object_value_item(std::deque<std::u8string>& names)
    {
        if (names.size() == 0)
        {
            return {};
        }

        if (m_setting_service == nullptr)
        {
            return {};
        }

        for (auto& ptree_item_ : m_setting_service->setting_user().items())
        {
            if (ptree_item_.name() == names.front())
            {
                if (ptree_item_.value().type() == typeid(std::deque<ptree_item>))
                {
                    names.pop_front();
                    return find_setting_user_object_value_item(names);
                }
                else
                {
                    return ptree_item_;
                }
            }
        }
        return {};
    }

    std::optional<std::reference_wrapper<ptree_item>> find_setting_tmp_object_value_item(std::deque<std::u8string>& names)
    {
        if (names.size() == 0)
        {
            return {};
        }

        if (m_setting_service == nullptr)
        {
            return {};
        }

        for (auto& ptree_item_ : m_setting_service->setting_tmp().items())
        {
            if (ptree_item_.name() == names.front())
            {
                if (ptree_item_.value().type() == typeid(std::deque<ptree_item>))
                {
                    names.pop_front();
                    return find_setting_tmp_object_value_item(names);
                }
                else
                {
                    return ptree_item_;
                }
            }
        }
        return {};
    }

private:
    std::deque<std::u8string> m_names;
    bool m_tmp;

    Setting* m_setting_service;
};

KIWI_FAST_CLOSE_PLUGIN_UTILITY_NAMESPACE

