#pragma once

#include <kiwi.fast.utility/detail/config.h>

#include <boost/asio.hpp>

#include <vector>
#include <thread>
#include <memory>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class threadpool
{
public:
    threadpool(int thread_num)
        : m_thread_num(thread_num), m_stopped(false), m_work(m_io)
    {
        for(int i = 0; i < m_thread_num; ++i)
        {
            m_threads.push_back(std::make_shared<std::thread>([&](){m_io.run();}));
        }
    }

    ~threadpool()
    {
        stop();
    }

    void stop()
    {
        if(!m_stopped)
        {
            m_io.stop();

            for(auto t : m_threads)
            {
                t->join();
            }

            m_stopped = true;
        }
    }

    template<typename F, typename... Args>
    void post(F&& f, Args&&... args)
    {
        m_io.post(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    }

private:
    bool m_stopped;
    std::vector<std::shared_ptr<std::thread>> m_threads;
    int m_thread_num;
    boost::asio::io_context m_io;
    boost::asio::io_context::work m_work;
};

class threadpool_per_cpu
{
public:
    threadpool_per_cpu()
        : m_stopped(false), m_work(m_io)
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        m_thread_num = info.dwNumberOfProcessors - 1;

        for(int i = 0; i < m_thread_num; ++i)
        {
            m_threads.push_back(std::make_shared<std::thread>([&](DWORD mask){

                m_cpu_mask = mask;

                SetThreadAffinityMask(GetCurrentThread(), mask);

                m_io.run();

            }, 1 << (i+1)));
        }
    }

    ~threadpool_per_cpu()
    {
        stop();
    }

    void stop()
    {
        if(!m_stopped)
        {
            m_io.stop();

            for(auto t : m_threads)
            {
                t->join();
            }

            m_stopped = true;
        }
    }

    template<typename F, typename... Args>
    void post(F&& f, Args&&... args)
    {
        m_io.post(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    }

    static int cpu_mask()
    {
        return m_cpu_mask;
    }

private:
    bool m_stopped;
    std::vector<std::shared_ptr<std::thread>> m_threads;
    int m_thread_num;
    boost::asio::io_context m_io;
    boost::asio::io_context::work m_work;

    static thread_local int m_cpu_mask;
};



KIWI_FAST_CLOSE_UTILITY_NAMESPACE
