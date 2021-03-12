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
        argv_[i] = strdup(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(argv[i]).c_str());
    }

    int result = ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv_ );

    for(int i = 0; i < argc; ++i)
    {
        free(argv_[i]);
    }
    delete[] argv_;

    return result;
}

/* boost.test没有提供wmain的定义 */
/* boost.test没有提供::boost::unit_test::unit_test_main的宽字符版本 */

BOOST_AUTO_TEST_SUITE(s_test)

BOOST_AUTO_TEST_CASE(c_test)
{
    //WARN CHECK REQUIRE
    int a = 5;
    BOOST_WARN((a != 5));//false

    int b = 5;
    BOOST_CHECK((a == b));//true

    int* p = nullptr;
    BOOST_REQUIRE((p == nullptr));//true


    //TEST_MESSAGE EQUAL/NE/GE/GT/LE/LT THROW/NO_THROW ...
    BOOST_TEST_MESSAGE("output message");

    BOOST_CHECK_EQUAL(a, b);


    //::boost::unit_test::framework::master_test_suite().argc
    //::boost::unit_test::framework::master_test_suite().argv


    //int* c = new int;
    //Detected memory leaks!
    //Dumping objects ->
    //{6392} normal block at 0x000002412F2162A0, 4 bytes long.
    // Data: <    > CD CD CD CD
    //Object dump complete.
}

//使用vs cmake时，有时会C++ IntelliSense工作异常，代码下都是红线，这时应该检查CMakeSettings.json中的intelliSenseMode，然后删除.vs文件夹并重新生成缓存

BOOST_AUTO_TEST_SUITE_END()
