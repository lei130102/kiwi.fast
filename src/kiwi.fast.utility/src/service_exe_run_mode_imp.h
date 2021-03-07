#pragma once

#include <kiwi.fast.utility/detail/config.h>

KIWI_FAST_OPEN_UTILITY_NAMESPACE

class service_exe_run_mode_imp
{
public:
    void set_interactive()
    {
        m_interactive = true;

        m_batch = false;
    }
    bool interactive()
    {
        return m_interactive;
    }
    void set_batch()
    {
        m_batch = true;

        m_interactive = false;
    }
    bool batch()
    {
        return m_batch;
    }

protected:
    service_exe_run_mode_imp()
        :m_interactive(false)
        , m_batch(false)
    {}

    virtual ~service_exe_run_mode_imp()
    {}

private:
    bool m_interactive;
    bool m_batch;
};

KIWI_FAST_CLOSE_UTILITY_NAMESPACE

#define SERVICE_EXE_RUN_MODE_ADAPTER_METHOD(imp_class)                                                        \
    void set_interactive()                                                                                    \
    {                                                                                                         \
        imp_class::set_interactive();                                                                         \
    }                                                                                                         \
    bool interactive()                                                                                        \
    {                                                                                                         \
        return imp_class::interactive();                                                                      \
    }                                                                                                         \
    void set_batch()                                                                                          \
    {                                                                                                         \
        imp_class::set_batch();                                                                               \
    }                                                                                                         \
    bool batch()                                                                                              \
    {																						                  \
        return imp_class::batch();                                                                            \
    }

