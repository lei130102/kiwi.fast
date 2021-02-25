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
    struct cengshen;

    template<>
    struct query_condition<cengshen>
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
    struct query_result<cengshen>
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
            //方区左上角纬度
            double lat;
            //方区左上角经度
            double lon;
            //总数
            std::uint64_t all;
            std::uint64_t d0;
            std::uint64_t d5;
            std::uint64_t d10;
            std::uint64_t d15;
            std::uint64_t d20;
            std::uint64_t d25;
            std::uint64_t d30;
            std::uint64_t d35;
            std::uint64_t d40;
            std::uint64_t d45;
            std::uint64_t d50;
            std::uint64_t d55;
            std::uint64_t d60;
            std::uint64_t d65;
            std::uint64_t d70;
            std::uint64_t d75;
            std::uint64_t d80;
            std::uint64_t d85;
            std::uint64_t d90;
            std::uint64_t d95;
            std::uint64_t d100;
            std::uint64_t d125;
            std::uint64_t d150;
            std::uint64_t d175;
            std::uint64_t d200;
            std::uint64_t d225;
            std::uint64_t d250;
            std::uint64_t d275;
            std::uint64_t d300;
            std::uint64_t d325;
            std::uint64_t d350;
            std::uint64_t d375;
            std::uint64_t d400;
            std::uint64_t d425;
            std::uint64_t d450;
            std::uint64_t d475;
            std::uint64_t d500;
            std::uint64_t d550;
            std::uint64_t d600;
            std::uint64_t d650;
            std::uint64_t d700;
            std::uint64_t d750;
            std::uint64_t d800;
            std::uint64_t d850;
            std::uint64_t d900;
            std::uint64_t d950;
            std::uint64_t d1000;
            std::uint64_t d1050;
            std::uint64_t d1100;
            std::uint64_t d1150;
            std::uint64_t d1200;
            std::uint64_t d1250;
            std::uint64_t d1300;
            std::uint64_t d1350;
            std::uint64_t d1400;
            std::uint64_t d1450;
            std::uint64_t d1500;
            std::uint64_t d1550;
            std::uint64_t d1600;
            std::uint64_t d1650;
            std::uint64_t d1700;
            std::uint64_t d1750;
            std::uint64_t d1800;
            std::uint64_t d1850;
            std::uint64_t d1900;
            std::uint64_t d1950;
            std::uint64_t d2000;
            std::uint64_t d2100;
            std::uint64_t d2200;
            std::uint64_t d2300;
            std::uint64_t d2400;
            std::uint64_t d2500;
            std::uint64_t d2600;
            std::uint64_t d2700;
            std::uint64_t d2800;
            std::uint64_t d2900;
            std::uint64_t d3000;
            std::uint64_t d3100;
            std::uint64_t d3200;
            std::uint64_t d3300;
            std::uint64_t d3400;
            std::uint64_t d3500;
            std::uint64_t d3600;
            std::uint64_t d3700;
            std::uint64_t d3800;
            std::uint64_t d3900;
            std::uint64_t d4000;
            std::uint64_t d4100;
            std::uint64_t d4200;
            std::uint64_t d4300;
            std::uint64_t d4400;
            std::uint64_t d4500;
            std::uint64_t d4600;
            std::uint64_t d4700;
            std::uint64_t d4800;
            std::uint64_t d4900;
            std::uint64_t d5000;
            std::uint64_t d5100;
            std::uint64_t d5200;
            std::uint64_t d5300;
            std::uint64_t d5400;
            std::uint64_t d5500;
            std::uint64_t d5600;
            std::uint64_t d5700;
            std::uint64_t d5800;
            std::uint64_t d5900;
            std::uint64_t d6000;
            std::uint64_t d6100;
            std::uint64_t d6200;
            std::uint64_t d6300;
            std::uint64_t d6400;
            std::uint64_t d6500;
            std::uint64_t d6600;
            std::uint64_t d6700;
            std::uint64_t d6800;
            std::uint64_t d6900;
            std::uint64_t d7000;
            std::uint64_t d7100;
            std::uint64_t d7200;
            std::uint64_t d7300;
            std::uint64_t d7400;
            std::uint64_t d7500;
            std::uint64_t d7600;
            std::uint64_t d7700;
            std::uint64_t d7800;
            std::uint64_t d7900;
            std::uint64_t d8000;
            std::uint64_t d8100;
            std::uint64_t d8200;
            std::uint64_t d8300;
            std::uint64_t d8400;
            std::uint64_t d8500;
            std::uint64_t d8600;
            std::uint64_t d8700;
            std::uint64_t d8800;
            std::uint64_t d8900;
            std::uint64_t d9000;
        };

        std::vector<item_type> items;
    };
}

struct cengshen;

template<>
class query_condition<cengshen> : protected detail::query_condition<detail::cengshen>
{
public:
    using base_type = detail::query_condition<detail::cengshen>;

public:
    void add(int year, int month, int element, int interval)
    {
        items.push_back(item_type{year, month, element, interval});
    }

    void to_property_tree(boost::property_tree::wptree& tree)
    {
        for(auto const& item : items)
        {
            boost::property_tree::wptree child;
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"year"), item.year);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"month"), item.month);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"element"), item.element);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"interval"), item.interval);
            tree.add_child(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"query_condition.item"), child);
        }
    }

    void from_xml(std::u8string const& xml)
    {
        std::wstringstream wsstream;
        wsstream << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(xml);

        boost::property_tree::wptree tree;
        boost::property_tree::xml_parser::read_xml(wsstream, tree);

        for(auto const& v : tree.get_child(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"query_condition")))
        {
            add(v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"year"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"month"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"element"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"interval"))
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
class query_result<cengshen> : protected detail::query_result<detail::cengshen>
{
public:
    using base_type = detail::query_result<detail::cengshen>;
    using this_type = query_result<cengshen>;

protected:
    void add(int year, int month, int element, int interval, double lat, double lon, std::uint64_t all
             , std::uint64_t d0
             , std::uint64_t d5
             , std::uint64_t d10
             , std::uint64_t d15
             , std::uint64_t d20
             , std::uint64_t d25
             , std::uint64_t d30
             , std::uint64_t d35
             , std::uint64_t d40
             , std::uint64_t d45
             , std::uint64_t d50
             , std::uint64_t d55
             , std::uint64_t d60
             , std::uint64_t d65
             , std::uint64_t d70
             , std::uint64_t d75
             , std::uint64_t d80
             , std::uint64_t d85
             , std::uint64_t d90
             , std::uint64_t d95
             , std::uint64_t d100
             , std::uint64_t d125
             , std::uint64_t d150
             , std::uint64_t d175
             , std::uint64_t d200
             , std::uint64_t d225
             , std::uint64_t d250
             , std::uint64_t d275
             , std::uint64_t d300
             , std::uint64_t d325
             , std::uint64_t d350
             , std::uint64_t d375
             , std::uint64_t d400
             , std::uint64_t d425
             , std::uint64_t d450
             , std::uint64_t d475
             , std::uint64_t d500
             , std::uint64_t d550
             , std::uint64_t d600
             , std::uint64_t d650
             , std::uint64_t d700
             , std::uint64_t d750
             , std::uint64_t d800
             , std::uint64_t d850
             , std::uint64_t d900
             , std::uint64_t d950
             , std::uint64_t d1000
             , std::uint64_t d1050
             , std::uint64_t d1100
             , std::uint64_t d1150
             , std::uint64_t d1200
             , std::uint64_t d1250
             , std::uint64_t d1300
             , std::uint64_t d1350
             , std::uint64_t d1400
             , std::uint64_t d1450
             , std::uint64_t d1500
             , std::uint64_t d1550
             , std::uint64_t d1600
             , std::uint64_t d1650
             , std::uint64_t d1700
             , std::uint64_t d1750
             , std::uint64_t d1800
             , std::uint64_t d1850
             , std::uint64_t d1900
             , std::uint64_t d1950
             , std::uint64_t d2000
             , std::uint64_t d2100
             , std::uint64_t d2200
             , std::uint64_t d2300
             , std::uint64_t d2400
             , std::uint64_t d2500
             , std::uint64_t d2600
             , std::uint64_t d2700
             , std::uint64_t d2800
             , std::uint64_t d2900
             , std::uint64_t d3000
             , std::uint64_t d3100
             , std::uint64_t d3200
             , std::uint64_t d3300
             , std::uint64_t d3400
             , std::uint64_t d3500
             , std::uint64_t d3600
             , std::uint64_t d3700
             , std::uint64_t d3800
             , std::uint64_t d3900
             , std::uint64_t d4000
             , std::uint64_t d4100
             , std::uint64_t d4200
             , std::uint64_t d4300
             , std::uint64_t d4400
             , std::uint64_t d4500
             , std::uint64_t d4600
             , std::uint64_t d4700
             , std::uint64_t d4800
             , std::uint64_t d4900
             , std::uint64_t d5000
             , std::uint64_t d5100
             , std::uint64_t d5200
             , std::uint64_t d5300
             , std::uint64_t d5400
             , std::uint64_t d5500
             , std::uint64_t d5600
             , std::uint64_t d5700
             , std::uint64_t d5800
             , std::uint64_t d5900
             , std::uint64_t d6000
             , std::uint64_t d6100
             , std::uint64_t d6200
             , std::uint64_t d6300
             , std::uint64_t d6400
             , std::uint64_t d6500
             , std::uint64_t d6600
             , std::uint64_t d6700
             , std::uint64_t d6800
             , std::uint64_t d6900
             , std::uint64_t d7000
             , std::uint64_t d7100
             , std::uint64_t d7200
             , std::uint64_t d7300
             , std::uint64_t d7400
             , std::uint64_t d7500
             , std::uint64_t d7600
             , std::uint64_t d7700
             , std::uint64_t d7800
             , std::uint64_t d7900
             , std::uint64_t d8000
             , std::uint64_t d8100
             , std::uint64_t d8200
             , std::uint64_t d8300
             , std::uint64_t d8400
             , std::uint64_t d8500
             , std::uint64_t d8600
             , std::uint64_t d8700
             , std::uint64_t d8800
             , std::uint64_t d8900
             , std::uint64_t d9000
             )
    {
        items.push_back(item_type{year, month, element, interval, lat, lon, all
                        ,d0
                        ,d5
                        ,d10
                        ,d15
                        ,d20
                        ,d25
                        ,d30
                        ,d35
                        ,d40
                        ,d45
                        ,d50
                        ,d55
                        ,d60
                        ,d65
                        ,d70
                        ,d75
                        ,d80
                        ,d85
                        ,d90
                        ,d95
                        ,d100
                        ,d125
                        ,d150
                        ,d175
                        ,d200
                        ,d225
                        ,d250
                        ,d275
                        ,d300
                        ,d325
                        ,d350
                        ,d375
                        ,d400
                        ,d425
                        ,d450
                        ,d475
                        ,d500
                        ,d550
                        ,d600
                        ,d650
                        ,d700
                        ,d750
                        ,d800
                        ,d850
                        ,d900
                        ,d950
                        ,d1000
                        ,d1050
                        ,d1100
                        ,d1150
                        ,d1200
                        ,d1250
                        ,d1300
                        ,d1350
                        ,d1400
                        ,d1450
                        ,d1500
                        ,d1550
                        ,d1600
                        ,d1650
                        ,d1700
                        ,d1750
                        ,d1800
                        ,d1850
                        ,d1900
                        ,d1950
                        ,d2000
                        ,d2100
                        ,d2200
                        ,d2300
                        ,d2400
                        ,d2500
                        ,d2600
                        ,d2700
                        ,d2800
                        ,d2900
                        ,d3000
                        ,d3100
                        ,d3200
                        ,d3300
                        ,d3400
                        ,d3500
                        ,d3600
                        ,d3700
                        ,d3800
                        ,d3900
                        ,d4000
                        ,d4100
                        ,d4200
                        ,d4300
                        ,d4400
                        ,d4500
                        ,d4600
                        ,d4700
                        ,d4800
                        ,d4900
                        ,d5000
                        ,d5100
                        ,d5200
                        ,d5300
                        ,d5400
                        ,d5500
                        ,d5600
                        ,d5700
                        ,d5800
                        ,d5900
                        ,d6000
                        ,d6100
                        ,d6200
                        ,d6300
                        ,d6400
                        ,d6500
                        ,d6600
                        ,d6700
                        ,d6800
                        ,d6900
                        ,d7000
                        ,d7100
                        ,d7200
                        ,d7300
                        ,d7400
                        ,d7500
                        ,d7600
                        ,d7700
                        ,d7800
                        ,d7900
                        ,d8000
                        ,d8100
                        ,d8200
                        ,d8300
                        ,d8400
                        ,d8500
                        ,d8600
                        ,d8700
                        ,d8800
                        ,d8900
                        ,d9000
                        });
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
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"year"), item.year);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"month"), item.month);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"element"), item.element);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"interval"), item.interval);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"lat"), item.lat);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"lon"), item.lon);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"all"), item.all);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d0"), item.d0);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5"), item.d5);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d10"), item.d10);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d15"), item.d15);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d20"), item.d20);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d25"), item.d25);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d30"), item.d30);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d35"), item.d35);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d40"), item.d40);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d45"), item.d45);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d50"), item.d50);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d55"), item.d55);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d60"), item.d60);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d65"), item.d65);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d70"), item.d70);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d75"), item.d75);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d80"), item.d80);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d85"), item.d85);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d90"), item.d90);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d95"), item.d95);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d100"), item.d100);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d125"), item.d125);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d150"), item.d150);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d175"), item.d175);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d200"), item.d200);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d225"), item.d225);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d250"), item.d250);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d275"), item.d275);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d300"), item.d300);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d325"), item.d325);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d350"), item.d350);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d375"), item.d375);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d400"), item.d400);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d425"), item.d425);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d450"), item.d450);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d475"), item.d475);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d500"), item.d500);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d550"), item.d550);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d600"), item.d600);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d650"), item.d650);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d700"), item.d700);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d750"), item.d750);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d800"), item.d800);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d850"), item.d850);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d900"), item.d900);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d950"), item.d950);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1000"), item.d1000);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1050"), item.d1050);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1100"), item.d1100);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1150"), item.d1150);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1200"), item.d1200);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1250"), item.d1250);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1300"), item.d1300);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1350"), item.d1350);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1400"), item.d1400);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1450"), item.d1450);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1500"), item.d1500);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1550"), item.d1550);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1600"), item.d1600);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1650"), item.d1650);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1700"), item.d1700);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1750"), item.d1750);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1800"), item.d1800);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1850"), item.d1850);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1900"), item.d1900);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1950"), item.d1950);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2000"), item.d2000);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2100"), item.d2100);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2200"), item.d2200);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2300"), item.d2300);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2400"), item.d2400);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2500"), item.d2500);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2600"), item.d2600);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2700"), item.d2700);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2800"), item.d2800);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2900"), item.d2900);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3000"), item.d3000);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3100"), item.d3100);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3200"), item.d3200);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3300"), item.d3300);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3400"), item.d3400);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3500"), item.d3500);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3600"), item.d3600);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3700"), item.d3700);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3800"), item.d3800);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3900"), item.d3900);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4000"), item.d4000);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4100"), item.d4100);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4200"), item.d4200);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4300"), item.d4300);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4400"), item.d4400);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4500"), item.d4500);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4600"), item.d4600);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4700"), item.d4700);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4800"), item.d4800);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4900"), item.d4900);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5000"), item.d5000);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5100"), item.d5100);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5200"), item.d5200);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5300"), item.d5300);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5400"), item.d5400);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5500"), item.d5500);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5600"), item.d5600);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5700"), item.d5700);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5800"), item.d5800);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5900"), item.d5900);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6000"), item.d6000);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6100"), item.d6100);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6200"), item.d6200);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6300"), item.d6300);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6400"), item.d6400);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6500"), item.d6500);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6600"), item.d6600);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6700"), item.d6700);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6800"), item.d6800);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6900"), item.d6900);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7000"), item.d7000);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7100"), item.d7100);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7200"), item.d7200);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7300"), item.d7300);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7400"), item.d7400);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7500"), item.d7500);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7600"), item.d7600);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7700"), item.d7700);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7800"), item.d7800);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7900"), item.d7900);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8000"), item.d8000);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8100"), item.d8100);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8200"), item.d8200);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8300"), item.d8300);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8400"), item.d8400);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8500"), item.d8500);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8600"), item.d8600);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8700"), item.d8700);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8800"), item.d8800);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8900"), item.d8900);
            child.add(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d9000"), item.d9000);
            tree.add_child(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"query_result.item"), child);
        }
    }

    void from_xml(std::u8string const& xml)
    {
        std::wstringstream wsstream;
        wsstream << KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(xml);

        boost::property_tree::wptree tree;
        boost::property_tree::xml_parser::read_xml(wsstream, tree);

        for(auto const& v : tree.get_child(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"query_result")))
        {
            add(v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"year"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"month"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"element"))
                ,v.second.get<int>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"interval"))
                ,v.second.get<double>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"lat"))
                ,v.second.get<double>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"lon"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"all"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d0"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d10"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d15"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d20"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d25"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d30"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d35"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d40"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d45"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d50"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d55"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d60"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d65"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d70"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d75"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d80"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d85"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d90"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d95"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d100"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d125"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d150"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d175"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d200"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d225"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d250"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d275"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d300"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d325"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d350"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d375"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d400"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d425"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d450"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d475"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d500"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d550"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d600"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d650"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d700"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d750"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d800"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d850"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d900"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d950"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1000"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1050"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1100"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1150"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1200"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1250"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1300"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1350"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1400"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1450"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1500"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1550"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1600"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1650"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1700"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1750"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1800"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1850"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1900"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d1950"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2000"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2100"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2200"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2300"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2400"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2500"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2600"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2700"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2800"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d2900"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3000"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3100"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3200"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3300"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3400"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3500"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3600"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3700"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3800"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d3900"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4000"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4100"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4200"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4300"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4400"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4500"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4600"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4700"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4800"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d4900"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5000"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5100"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5200"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5300"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5400"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5500"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5600"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5700"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5800"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d5900"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6000"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6100"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6200"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6300"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6400"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6500"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6600"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6700"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6800"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d6900"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7000"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7100"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7200"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7300"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7400"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7500"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7600"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7700"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7800"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d7900"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8000"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8100"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8200"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8300"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8400"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8500"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8600"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8700"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8800"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d8900"))
                ,v.second.get<std::uint64_t>(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_wide(u8"d9000"))
                );
        }
    }

    template<typename F, typename... Args>
    void handle(F&& f, Args&&... args)
    {
        for(auto const& item : items)
        {
            std::bind(f, item.year, item.month, item.element, item.interval, item.lat, item.lon, item.all
                      , item.d0
                      , item.d5
                      , item.d10
                      , item.d15
                      , item.d20
                      , item.d25
                      , item.d30
                      , item.d35
                      , item.d40
                      , item.d45
                      , item.d50
                      , item.d55
                      , item.d60
                      , item.d65
                      , item.d70
                      , item.d75
                      , item.d80
                      , item.d85
                      , item.d90
                      , item.d95
                      , item.d100
                      , item.d125
                      , item.d150
                      , item.d175
                      , item.d200
                      , item.d225
                      , item.d250
                      , item.d275
                      , item.d300
                      , item.d325
                      , item.d350
                      , item.d375
                      , item.d400
                      , item.d425
                      , item.d450
                      , item.d475
                      , item.d500
                      , item.d550
                      , item.d600
                      , item.d650
                      , item.d700
                      , item.d750
                      , item.d800
                      , item.d850
                      , item.d900
                      , item.d950
                      , item.d1000
                      , item.d1050
                      , item.d1100
                      , item.d1150
                      , item.d1200
                      , item.d1250
                      , item.d1300
                      , item.d1350
                      , item.d1400
                      , item.d1450
                      , item.d1500
                      , item.d1550
                      , item.d1600
                      , item.d1650
                      , item.d1700
                      , item.d1750
                      , item.d1800
                      , item.d1850
                      , item.d1900
                      , item.d1950
                      , item.d2000
                      , item.d2100
                      , item.d2200
                      , item.d2300
                      , item.d2400
                      , item.d2500
                      , item.d2600
                      , item.d2700
                      , item.d2800
                      , item.d2900
                      , item.d3000
                      , item.d3100
                      , item.d3200
                      , item.d3300
                      , item.d3400
                      , item.d3500
                      , item.d3600
                      , item.d3700
                      , item.d3800
                      , item.d3900
                      , item.d4000
                      , item.d4100
                      , item.d4200
                      , item.d4300
                      , item.d4400
                      , item.d4500
                      , item.d4600
                      , item.d4700
                      , item.d4800
                      , item.d4900
                      , item.d5000
                      , item.d5100
                      , item.d5200
                      , item.d5300
                      , item.d5400
                      , item.d5500
                      , item.d5600
                      , item.d5700
                      , item.d5800
                      , item.d5900
                      , item.d6000
                      , item.d6100
                      , item.d6200
                      , item.d6300
                      , item.d6400
                      , item.d6500
                      , item.d6600
                      , item.d6700
                      , item.d6800
                      , item.d6900
                      , item.d7000
                      , item.d7100
                      , item.d7200
                      , item.d7300
                      , item.d7400
                      , item.d7500
                      , item.d7600
                      , item.d7700
                      , item.d7800
                      , item.d7900
                      , item.d8000
                      , item.d8100
                      , item.d8200
                      , item.d8300
                      , item.d8400
                      , item.d8500
                      , item.d8600
                      , item.d8700
                      , item.d8800
                      , item.d8900
                      , item.d9000
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
        item.all += count_item.all;
        item.d0 += count_item.d0;
        item.d5 += count_item.d5;
        item.d10 += count_item.d10;
        item.d15 += count_item.d15;
        item.d20 += count_item.d20;
        item.d25 += count_item.d25;
        item.d30 += count_item.d30;
        item.d35 += count_item.d35;
        item.d40 += count_item.d40;
        item.d45 += count_item.d45;
        item.d50 += count_item.d50;
        item.d55 += count_item.d55;
        item.d60 += count_item.d60;
        item.d65 += count_item.d65;
        item.d70 += count_item.d70;
        item.d75 += count_item.d75;
        item.d80 += count_item.d80;
        item.d85 += count_item.d85;
        item.d90 += count_item.d90;
        item.d95 += count_item.d95;
        item.d100 += count_item.d100;
        item.d125 += count_item.d125;
        item.d150 += count_item.d150;
        item.d175 += count_item.d175;
        item.d200 += count_item.d200;
        item.d225 += count_item.d225;
        item.d250 += count_item.d250;
        item.d275 += count_item.d275;
        item.d300 += count_item.d300;
        item.d325 += count_item.d325;
        item.d350 += count_item.d350;
        item.d375 += count_item.d375;
        item.d400 += count_item.d400;
        item.d425 += count_item.d425;
        item.d450 += count_item.d450;
        item.d475 += count_item.d475;
        item.d500 += count_item.d500;
        item.d550 += count_item.d550;
        item.d600 += count_item.d600;
        item.d650 += count_item.d650;
        item.d700 += count_item.d700;
        item.d750 += count_item.d750;
        item.d800 += count_item.d800;
        item.d850 += count_item.d850;
        item.d900 += count_item.d900;
        item.d950 += count_item.d950;
        item.d1000 += count_item.d1000;
        item.d1050 += count_item.d1050;
        item.d1100 += count_item.d1100;
        item.d1150 += count_item.d1150;
        item.d1200 += count_item.d1200;
        item.d1250 += count_item.d1250;
        item.d1300 += count_item.d1300;
        item.d1350 += count_item.d1350;
        item.d1400 += count_item.d1400;
        item.d1450 += count_item.d1450;
        item.d1500 += count_item.d1500;
        item.d1550 += count_item.d1550;
        item.d1600 += count_item.d1600;
        item.d1650 += count_item.d1650;
        item.d1700 += count_item.d1700;
        item.d1750 += count_item.d1750;
        item.d1800 += count_item.d1800;
        item.d1850 += count_item.d1850;
        item.d1900 += count_item.d1900;
        item.d1950 += count_item.d1950;
        item.d2000 += count_item.d2000;
        item.d2100 += count_item.d2100;
        item.d2200 += count_item.d2200;
        item.d2300 += count_item.d2300;
        item.d2400 += count_item.d2400;
        item.d2500 += count_item.d2500;
        item.d2600 += count_item.d2600;
        item.d2700 += count_item.d2700;
        item.d2800 += count_item.d2800;
        item.d2900 += count_item.d2900;
        item.d3000 += count_item.d3000;
        item.d3100 += count_item.d3100;
        item.d3200 += count_item.d3200;
        item.d3300 += count_item.d3300;
        item.d3400 += count_item.d3400;
        item.d3500 += count_item.d3500;
        item.d3600 += count_item.d3600;
        item.d3700 += count_item.d3700;
        item.d3800 += count_item.d3800;
        item.d3900 += count_item.d3900;
        item.d4000 += count_item.d4000;
        item.d4100 += count_item.d4100;
        item.d4200 += count_item.d4200;
        item.d4300 += count_item.d4300;
        item.d4400 += count_item.d4400;
        item.d4500 += count_item.d4500;
        item.d4600 += count_item.d4600;
        item.d4700 += count_item.d4700;
        item.d4800 += count_item.d4800;
        item.d4900 += count_item.d4900;
        item.d5000 += count_item.d5000;
        item.d5100 += count_item.d5100;
        item.d5200 += count_item.d5200;
        item.d5300 += count_item.d5300;
        item.d5400 += count_item.d5400;
        item.d5500 += count_item.d5500;
        item.d5600 += count_item.d5600;
        item.d5700 += count_item.d5700;
        item.d5800 += count_item.d5800;
        item.d5900 += count_item.d5900;
        item.d6000 += count_item.d6000;
        item.d6100 += count_item.d6100;
        item.d6200 += count_item.d6200;
        item.d6300 += count_item.d6300;
        item.d6400 += count_item.d6400;
        item.d6500 += count_item.d6500;
        item.d6600 += count_item.d6600;
        item.d6700 += count_item.d6700;
        item.d6800 += count_item.d6800;
        item.d6900 += count_item.d6900;
        item.d7000 += count_item.d7000;
        item.d7100 += count_item.d7100;
        item.d7200 += count_item.d7200;
        item.d7300 += count_item.d7300;
        item.d7400 += count_item.d7400;
        item.d7500 += count_item.d7500;
        item.d7600 += count_item.d7600;
        item.d7700 += count_item.d7700;
        item.d7800 += count_item.d7800;
        item.d7900 += count_item.d7900;
        item.d8000 += count_item.d8000;
        item.d8100 += count_item.d8100;
        item.d8200 += count_item.d8200;
        item.d8300 += count_item.d8300;
        item.d8400 += count_item.d8400;
        item.d8500 += count_item.d8500;
        item.d8600 += count_item.d8600;
        item.d8700 += count_item.d8700;
        item.d8800 += count_item.d8800;
        item.d8900 += count_item.d8900;
        item.d9000 += count_item.d9000;
    }
};

KIWI_FAST_CLOSE_MODEL_NAMESPACE
