#pragma once

#include <kiwi.fast.model/detail/config.h>

#include <kiwi.fast.model/detail/query_condition.h>
#include <kiwi.fast.model/detail/task_parameter.h>

#include <kiwi.fast.plugin_utility/code_conversion.h>
#include <kiwi.fast.plugin_utility/exceptions.h>

#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

KIWI_FAST_OPEN_MODEL_NAMESPACE

namespace detail
{

}

template<typename Tag>
class producer_consumer
{
public:
    using input_type = query_condition<Tag>;
    using msg_type = task_parameter<Tag>;

    producer_consumer(input_type&& input)
        : m_input(std::move(input))
    {
    }

    void producer_thread()
    {


        //for(auto const& iter = m_msg_list.begin(); iter != m_msg_list.end(); ++iter)
        {
            std::unique_lock<std::mutex> lock_(m_mutex);
            //m_msg_list.push_back(...);
            //m_condition_variable.notify_all();
        }
    }

    void consumer_thread()
    {
        while(true)
        {
            std::unique_lock<std::mutex> lock_(m_mutex);
            m_condition_variable.wait(lock_, [this]{
                if(!m_msg_list.empty())
                {
                    return true;
                }
                else
                {
                    return false;
                }
            });

            msg_type msg_ = m_msg_list.front();
            m_msg_list.pop_front();

            //使用msg_
            //...



            //lock_.unlock();//我认为不需要再手动解锁
        }
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_condition_variable;

    std::list<msg_type> m_msg_list;

    input_type m_input;
};

KIWI_FAST_CLOSE_MODEL_NAMESPACE
