#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include <kiwi.fast.plugin_utility/code_conversion.h>

/* 自定义的初始化函数 */
bool init_unit_test()
{
    return true;
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    char** argv_ = new char*[argc];
    for(int i = 0; i < argc; ++i)
    {
        argv_[i] = strdup(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER to_local(argv[i]).c_str());
    }

    return ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv_ );

    for(int i = 0; i < argc; ++i)
    {
        free(argv_[i]);
    }
    delete[] argv_;
}

/* boost.test没有提供wmain的定义 */
/* boost.test没有提供::boost::unit_test::unit_test_main的宽字符版本 */

BOOST_AUTO_TEST_SUITE(s_test)

BOOST_AUTO_TEST_CASE(c_test)
{
    BOOST_CHECK(1==1);
}

BOOST_AUTO_TEST_SUITE_END()
