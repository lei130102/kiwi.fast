#pragma once

#include <kiwi.fast.model/detail/config.h>

#include <kiwi.fast.model/detail/task_parameter.h>
#include <kiwi.fast.model/detail/query_condition.h>
#include <kiwi.fast.model/detail/query_result.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <vector>
#include <functional>

#include <cstdint>
#include <cmath>

KIWI_FAST_OPEN_MODEL_NAMESPACE

namespace detail
{
    struct tongcun;

    template<>
    struct query_condition<tongcun>
    {
        struct item_type
        {
            //年 1990-1990年
            int year;
            //月 1-一月
            int month;
            //分辨率 2-2° 5-5°
            int interval;
        };

        std::vector<item_type> items;
    };

    template<>
    struct query_result<tongcun>
    {
        struct item_type
        {
            //年 1990-1990年
            int year;
            //月 1-一月
            int month;
            //分辨率 2-2° 5-5°
            int interval;
            //方区左上角纬度
            double lat;
            //方区左上角经度
            double lon;
            //温盐同存站次数
            std::uint64_t tongcun_count;
            //总站次数
            std::uint64_t total_count;
        };

        std::vector<item_type> items;
    };
}

struct tongcun;

template<>
class query_condition<tongcun> : protected detail::query_condition<detail::tongcun>
{
public:
    using base_type = detail::query_condition<detail::tongcun>;

public:
    query_condition()
    {
        //注意对结构体的初始化
    }

    void add(int year, int month, int interval)
    {
        items.push_back(item_type{year, month, interval});
    }

    void to_property_tree(boost::property_tree::wptree& tree)
    {
        for(auto const& item : items)
        {
            boost::property_tree::wptree child;
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"year"), item.year);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"month"), item.month);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"interval"), item.interval);
            tree.add_child(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"query_condition.item"), child);
        }
    }

    void from_xml(std::u8string const& xml)
    {
        std::wstringstream wsstream;
        wsstream << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(xml);

        boost::property_tree::wptree tree;
        boost::property_tree::xml_parser::read_xml(wsstream, tree);

        for(auto const& v : tree.get_child(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"query_condition")))
        {
            add(v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"year"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"month"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"interval"))
                );
        }
    }

    template<typename F, typename... Args>
    void handle(F&& f, Args&&... args)
    {
        for(auto const& item : items)
        {
            std::bind(f, item.year, item.month, item.interval, args...)();
        }
    }
};

template<>
class query_result<tongcun> : protected detail::query_result<detail::tongcun>
{
public:
    using base_type = detail::query_result<detail::tongcun>;
    using this_type = query_result<tongcun>;

public:
    query_result()
    {
        //注意对结构体的初始化
    }

    void add(int year, int month, int interval, double lat, double lon, std::uint64_t tongcun_count, std::uint64_t total_count)
    {
        items.push_back(item_type{year, month, interval, lat, lon, tongcun_count, total_count});
    }

    std::size_t size()
    {
        return items.size();
    }

    void merge(this_type const& input_query_result)
    {
        for(auto const& input_item : input_query_result.items)
        {
            bool is_added = false;
            for(auto& item : items)
            {
                if(is_same_location(input_item, item))
                {
                    add_count(item, input_item);
                    is_added = true;
                    break;
                }
            }
            if(!is_added)
            {
                items.push_back(input_item);
            }
        }
    }

    void to_property_tree(boost::property_tree::wptree& tree)
    {
        for(auto const& item : items)
        {
            boost::property_tree::wptree child;
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"year"), item.year);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"month"), item.month);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"interval"), item.interval);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"lat"), item.lat);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"lon"), item.lon);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"tongcun_count"), item.tongcun_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"total_count"), item.total_count);
            tree.add_child(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"query_result.item"), child);
        }
    }

    void from_xml(std::u8string const& xml)
    {
        std::wstringstream wsstream;
        wsstream << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(xml);

        boost::property_tree::wptree tree;
        boost::property_tree::xml_parser::read_xml(wsstream, tree);

        for(auto const& v : tree.get_child(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"query_result")))
        {
            add(v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"year"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"month"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"interval"))
                ,v.second.get<double>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"lat"))
                ,v.second.get<double>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"lon"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"tongcun_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"total_count"))
                );
        }
    }

    template<typename F, typename... Args>
    void handle(F&& f, Args&&... args)
    {
        for(auto const& item : items)
        {
            std::bind(f, item.year, item.month, item.interval, item.lat, item.lon, item.tongcun_count, item.total_count, args...)();
        }
    }

    static bool is_same_location(this_type::item_type const& lhs, this_type::item_type const& rhs)
    {
        if((std::fabs(lhs.lat - rhs.lat) < 1e-3)
                && (std::fabs(lhs.lon - rhs.lon) < 1e-3))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static void add_count(this_type::item_type& item, this_type::item_type const& count_item)
    {
        item.tongcun_count += count_item.tongcun_count;
        item.total_count += count_item.total_count;
    }
};

KIWI_FAST_CLOSE_MODEL_NAMESPACE
