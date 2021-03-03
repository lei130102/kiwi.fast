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
    struct tongguo;

    template<>
    struct query_condition<tongguo>
    {
        struct item_type
        {
            //年 1990-1990年
            int year;
            //月 1-一月
            int month;
            //要素 0-温度 1-盐度
            int element;
            //分辨率 2-2° 5-5°
            int interval;
        };

        std::vector<item_type> items;
    };

    template<>
    struct query_result<tongguo>
    {
        struct item_type
        {
            //年 1990-1990年
            int year;
            //月 1-一月
            int month;
            //要素 0-温度 1-盐度
            int element;
            //分辨率 2-2° 5-5°
            int interval;
            //设备类型
            std::u8string instrument_type;
            //方区左上角纬度
            double lat;
            //方区左上角经度
            double lon;
            //
            std::uint64_t latlonrange_count;
            std::uint64_t landingdiscrimination_count;
            std::uint64_t time_count;
            std::uint64_t standardlayerdensitystability_count;
            std::uint64_t standardlayertemperaturesalinitystatistics_count;
            std::uint64_t measuredlayerdensitystability_count;
            std::uint64_t temperaturesalinitygradient_count;
            std::uint64_t incremental_count;
            std::uint64_t illegalcode_count;
            std::uint64_t levelreversalrepetition_count;
            std::uint64_t extremum_count;
            //总个数
            std::uint64_t total_count;
        };

        std::vector<item_type> items;
    };
}

struct tongguo;

template<>
class query_condition<tongguo> : protected detail::query_condition<detail::tongguo>
{
public:
    using base_type = detail::query_condition<detail::tongguo>;

public:
    query_condition()
    {
        //注意对结构体的初始化
    }

    void add(int year, int month, int element, int interval)
    {
        items.push_back(item_type{year, month, element, interval});
    }

    void to_property_tree(boost::property_tree::wptree& tree)
    {
        for(auto const& item : items)
        {
            boost::property_tree::wptree child;
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"year"), item.year);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"month"), item.month);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"element"), item.element);
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
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"element"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"interval"))
                );
        }
    }

    template<typename F, typename... Args>
    void handle(F&& f, Args&&... args)
    {
        for(auto const& item : items)
        {
            std::bind(f, item.year, item.month, item.element, item.interval, args...)();
        }
    }
};

template<>
class query_result<tongguo> : protected detail::query_result<detail::tongguo>
{
public:
    using base_type = detail::query_result<detail::tongguo>;
    using this_type = query_result<tongguo>;

public:
    query_result()
    {
        //注意对结构体的初始化
    }

    void add(int year
             , int month
             , int element
             , int interval
             , std::u8string const& instrument_type
             , double lat
             , double lon
             , std::uint64_t latlonrange_count
             , std::uint64_t landingdiscrimination_count
             , std::uint64_t time_count
             , std::uint64_t standardlayerdensitystability_count
             , std::uint64_t standardlayertemperaturesalinitystatistics_count
             , std::uint64_t measuredlayerdensitystability_count
             , std::uint64_t temperaturesalinitygradient_count
             , std::uint64_t incremental_count
             , std::uint64_t illegalcode_count
             , std::uint64_t levelreversalrepetition_count
             , std::uint64_t extremum_count
             , std::uint64_t total_count)
    {
        items.push_back(item_type{year
                                  , month
                                  , element
                                  , interval
                                  , instrument_type
                                  , lat
                                  , lon
                                  , latlonrange_count
                                  , landingdiscrimination_count
                                  , time_count
                                  , standardlayerdensitystability_count
                                  , standardlayertemperaturesalinitystatistics_count
                                  , measuredlayerdensitystability_count
                                  , temperaturesalinitygradient_count
                                  , incremental_count
                                  , illegalcode_count
                                  , levelreversalrepetition_count
                                  , extremum_count
                                  , total_count
                        });
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
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"element"), item.element);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"interval"), item.interval);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"instrument_type"), KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(item.instrument_type));
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"lat"), item.lat);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"lon"), item.lon);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"latlonrange_count"), item.latlonrange_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"landingdiscrimination_count"), item.landingdiscrimination_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"time_count"), item.time_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"standardlayerdensitystability_count"), item.standardlayerdensitystability_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"standardlayertemperaturesalinitystatistics_count"), item.standardlayertemperaturesalinitystatistics_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"measuredlayerdensitystability_count"), item.measuredlayerdensitystability_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"temperaturesalinitygradient_count"), item.temperaturesalinitygradient_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"incremental_count"), item.incremental_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"illegalcode_count"), item.illegalcode_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"levelreversalrepetition_count"), item.levelreversalrepetition_count);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"extremum_count"), item.extremum_count);
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
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"element"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"interval"))
                ,KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(v.second.get<std::wstring>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"instrument_type")))
                ,v.second.get<double>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"lat"))
                ,v.second.get<double>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"lon"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"latlonrange_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"landingdiscrimination_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"time_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"standardlayerdensitystability_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"standardlayertemperaturesalinitystatistics_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"measuredlayerdensitystability_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"temperaturesalinitygradient_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"incremental_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"illegalcode_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"levelreversalrepetition_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"extremum_count"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(u8"total_count"))
                );
        }
    }

    template<typename F, typename... Args>
    void handle(F&& f, Args&&... args)
    {
        for(auto const& item : items)
        {
            std::bind(f
                      , item.year
                      , item.month
                      , item.element
                      , item.interval
                      , item.instrument_type
                      , item.lat
                      , item.lon
                      , item.latlonrange_count
                      , item.landingdiscrimination_count
                      , item.time_count
                      , item.standardlayerdensitystability_count
                      , item.standardlayertemperaturesalinitystatistics_count
                      , item.measuredlayerdensitystability_count
                      , item.temperaturesalinitygradient_count
                      , item.incremental_count
                      , item.illegalcode_count
                      , item.levelreversalrepetition_count
                      , item.extremum_count
                      , item.total_count
                      , args...)();
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
        item.latlonrange_count += count_item.latlonrange_count;
        item.landingdiscrimination_count += count_item.landingdiscrimination_count;
        item.time_count += count_item.time_count;
        item.standardlayerdensitystability_count += count_item.standardlayerdensitystability_count;
        item.standardlayertemperaturesalinitystatistics_count += count_item.standardlayertemperaturesalinitystatistics_count;
        item.measuredlayerdensitystability_count += count_item.measuredlayerdensitystability_count;
        item.temperaturesalinitygradient_count += count_item.temperaturesalinitygradient_count;
        item.incremental_count += count_item.incremental_count;
        item.illegalcode_count += count_item.illegalcode_count;
        item.levelreversalrepetition_count += count_item.levelreversalrepetition_count;
        item.extremum_count += count_item.extremum_count;

        item.total_count += count_item.total_count;
    }
};

KIWI_FAST_CLOSE_MODEL_NAMESPACE

