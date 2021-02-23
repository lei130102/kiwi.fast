#include <iostream>
#include <new>

#include <kiwi.fast.plugin_utility/detail/config.h>

#include <kiwi.fast.plugin_utility/exceptions.h>

void bad_alloc()
{
    //#include <new>
    //std::bad_alloc 是分配函数作为异常抛出的对象类型，以报告存储分配失败。

    //派生自std::exception

    //bad_alloc() noexcept;
    //bad_alloc( const bad_alloc& other ) noexcept;
    //virtual const char* what() const noexcept;








//    try
//    {
//        while(true)
//        {
//            new int[100000000ul];
//        }
//    }
//    catch(std::bad_alloc const& e)
//    {
//        std::cout << e.what() << '\n';
//    }
    /*
    output:

    std::bad_alloc
    */







//    while(true)
//    {
//        new int[100000000ul];
//    }

    /*
    output:

    std::bad_alloc
    terminate called after throwing an instance of 'std::bad_alloc'
        what():  std::bad_alloc
    */









//    try
//    {
//        try
//        {
//            KIWI_FAST_THROW(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER bad_alloc);
//        }
//        catch(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER bad_alloc& e)
//        {
//            /* 获取异常内存储的信息 */
//            std::cout << *boost::get_error_info<boost::throw_file>(e) << '\n';
//            std::cout << *boost::get_error_info<boost::throw_line>(e) << '\n';
//            std::cout << e.what() << '\n';

//            /* 向异常追加信息 */
//            e << boost::throw_function(__FUNCTION__);

//            /* 再次抛出异常 */
//            throw;
//        }
//    }
//    catch(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER bad_alloc& e)
//    {
//        std::cout << *boost::get_error_info<boost::throw_function>(e) << '\n';
//    }
    /*
    output:

    D:\OtherDocuments\kiwi.fast\src\test\test_exception.cpp
    67

    bad_alloc
    */








//    KIWI_FAST_THROW(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER bad_alloc);
    /*
    output:

    terminate called after throwing an instance of 'boost::wrapexcept<kiwi_fast::v1::plugin_utility::bad_alloc>'
      what():
    */










//    try
//    {
//        KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER bad_alloc, u8"这是一个中文异常描述");
//    }
//    catch(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER bad_alloc& e)
//    {
//        /* 获取异常内存储的信息 */
//        std::cout << *boost::get_error_info<boost::throw_file>(e) << '\n';
//        std::cout << *boost::get_error_info<boost::throw_line>(e) << '\n';
//        std::cout << e.what() << '\n';
//    }
    /*
    output:

    D:\OtherDocuments\kiwi.fast\src\test\test_exception.cpp
    129
    这是一个中文异常描述
    */











//    KIWI_FAST_THROW_DESCR(KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER bad_alloc, u8"这是一个中文异常描述");
    /*
    output:

    terminate called after throwing an instance of 'boost::wrapexcept<kiwi_fast::v1::plugin_utility::bad_alloc>'
      what():  这是一个中文异常描述
    */

}

int wmain(int argc, wchar_t* argv[])
{
    bad_alloc();

    return 0;
}
