#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include <test/test_code_convert.h>

#include <type_traits>

#include <kiwi.fast.plugin_utility/code_conversion.h>

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

    return ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv_ );

    for(int i = 0; i < argc; ++i)
    {
        free(argv_[i]);
    }
    delete[] argv_;
}

/*

"编码"

(以下跟计算机存储无关，但注意内存字节序)

Unicode编码:
00007F16 00007801

UTF8编码:
E7BC96 E7A081

UTF16BE编码:
FEFF 7F16 7801

UTF16LE编码:
FFFE 167F 0178

UTF32BE编码:
0000FEFF 00007F16 00007801

UTF32LE编码:
FFFE0000 167F00000 1780000

GB2312编码:
B1E0 C2EB

GBK编码:
B1E0 C2EB



windows平台:(window平台的终端编码一般是GBK)
std::wstring 等同于 std::u16string
wchar_t      等同于 char16_t         (UTF-16编码的子集，因为UTF-16是多字节编码，需要2个字节(绝大多数)或者4个字节，而一个char16_t只是2个字节)

linux平台:
std::wstring 等同于 std::u32string
wchar_t      等同于 char32_t         (UTF-32编码)

*/

BOOST_AUTO_TEST_SUITE(s_test_code_convert)

BOOST_AUTO_TEST_CASE(c_test_code)
{
    using namespace test_code_convert;

    auto p1 = "编码";
    BOOST_CHECK((std::is_same_v<decltype(p1), const char*>));

    print_hex(p1);
    /*
    E7 BC 96 E7 A0 81            (mingw-x64)
    B1 E0 C2 EB                  (_MSC_VER == 1928)
    */

    auto p2 = L"编码";
    BOOST_CHECK((std::is_same_v<decltype(p2), const wchar_t*>));

    print_hex(p2);
    /*
    16 7F 01 78                  (mingw-x64          L实现了utf8编码的const char*到const wchar_t*的转换，即L是平台依赖的)
    16 7F 01 78                  (_MSC_VER == 1928   L实现了本地编码的const char*到const wchar_t*的转换，即L是平台依赖的)
    */

    auto p3 = u8"编码";
    auto p3_ = u8R"(编码)";
    BOOST_CHECK((std::is_same_v<decltype(p3), const char8_t*>)); /* 注意以前是const char*，自从有了char8_t，就变为const char8_t* */

    print_hex(p3);
    /*
    E7 BC 96 E7 A0 81            (mingw-x64          u8实现了utf8编码的const char*到const char8_t*的转换)
    E7 BC 96 E7 A0 81            (_MSC_VER == 1928   u8实现了utf8编码的const char*(内部实现本地编码到utf8编码转换)到const char8_t*的转换)
    */


    std::cout << std::string({'\xb1', '\xe0', '\xc2', '\xeb'}) << '\n';
    /*
    编码                          (mingw-x64)
    编码                          (_MSC_VER == 1928)
    */

}

BOOST_AUTO_TEST_CASE(c_test_codecvt)
{
    /*
    mingw-x64中std::codecvt<wchar_t, char, std::mbstate_t>特化有问题，所以需要创建派生类重写虚函数
    */


    //in
    //char*->wchar_t*
    {
        std::string external = {'\xb1','\xe0','\xc2','\xeb'};        //B1 E0 C2 EB 00     (GBK编码)
        std::mbstate_t mb = std::mbstate_t();
        std::wstring buffer(4 * external.size(), '\0');
        const char* from_next = nullptr;
        wchar_t* to_next = nullptr;
        std::codecvt_base::result result = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER detail::codecvt_char_wchar_t().in(mb,
                                                                                        &external[0], &external[external.size()], from_next,
                                                                                        &buffer[0], &buffer[buffer.size()], to_next);
        if(result == std::codecvt_base::ok)
        {
            std::wstring internal;
            internal.append(buffer.data(), to_next);

            BOOST_CHECK(internal == std::wstring(L"编码"));
        }
        else if(result == std::codecvt_base::partial)
        {
            buffer.resize(to_next - &buffer[0]);

            std::wstring internal;
            internal.append(buffer.data(), to_next);

            BOOST_CHECK(internal == L"编码");
        }
    }

    //out
    //wchar_t*->char*
    {
        std::wstring internal = L"编码";
        std::mbstate_t mb{};
        std::string buffer(4 * internal.size(), '\0');
        const wchar_t* from_next = nullptr;
        char* to_next = nullptr;
        std::codecvt_base::result result = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER detail::codecvt_char_wchar_t().out(mb,
                                                                                       &internal[0], &internal[internal.size()], from_next,
                                                                                       &buffer[0], &buffer[buffer.size()], to_next);
        if(result == std::codecvt_base::ok)
        {
            std::string external;
            external.append(buffer.data(), to_next);

            BOOST_CHECK(external == std::string({'\xb1','\xe0','\xc2','\xeb'}));
        }
        else if(result == std::codecvt_base::partial)
        {
            buffer.resize(to_next - &buffer[0]);

            std::string external;
            external.append(buffer.data(), to_next);

            BOOST_CHECK(external == std::string({'\xb1','\xe0','\xc2','\xeb'}));
        }
    }

    //in
    //char8_t*->wchar_t*
    {
        std::u8string external = u8"编码";                  //E7 BC 96 E7 A0 81 00     (UTF8编码)
        std::mbstate_t mb = std::mbstate_t();
        std::wstring buffer(4 * external.size(), '\0');
        const char8_t* from_next = nullptr;
        wchar_t* to_next = nullptr;
        std::codecvt_base::result result = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER detail::codecvt_char8_t_wchar_t().in(mb,
                                                                                        &external[0], &external[external.size()], from_next,
                                                                                        &buffer[0], &buffer[buffer.size()], to_next);
        if(result == std::codecvt_base::ok)
        {
            std::wstring internal;
            internal.append(buffer.data(), to_next);

            BOOST_CHECK(internal == L"编码");
        }
        else if(result == std::codecvt_base::partial)
        {
            buffer.resize(to_next - &buffer[0]);

            std::wstring internal;
            internal.append(buffer.data(), to_next);

            BOOST_CHECK(internal == L"编码");
        }
    }

    //out
    //wchar_t*->char8_t
    {
        std::wstring internal = L"编码";
        std::mbstate_t mb{};
        std::u8string buffer(4 * internal.size(), '\0');
        const wchar_t* from_next = nullptr;
        char8_t* to_next = nullptr;
        std::codecvt_base::result result = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER detail::codecvt_char8_t_wchar_t().out(mb,
                                                                                       &internal[0], &internal[internal.size()], from_next,
                                                                                       &buffer[0], &buffer[buffer.size()], to_next);
        if(result == std::codecvt_base::ok)
        {
            std::u8string external;
            external.append(buffer.data(), to_next);

            BOOST_CHECK(external == u8"编码");
        }
        else if(result == std::codecvt_base::partial)
        {
            buffer.resize(to_next - &buffer[0]);

            std::u8string external;
            external.append(buffer.data(), to_next);

            BOOST_CHECK(external == u8"编码");
        }
    }
}

BOOST_AUTO_TEST_CASE(c_test_to)
{
    //string -> wstring
    {
        std::string input = {'\xb1','\xe0','\xc2','\xeb'};        //B1 E0 C2 EB 00     (GBK编码)
        std::wstring result = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(input);
        BOOST_CHECK(result == L"编码");
    }

    //wstring -> string
    {
        std::wstring input = L"编码";
        std::string result = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char>(input);
        BOOST_CHECK(result == std::string({'\xb1','\xe0','\xc2','\xeb'}));
    }

    //u8string -> wstring
    {
        std::u8string input = u8"编码";                  //E7 BC 96 E7 A0 81 00     (UTF8编码)
        std::wstring result = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<wchar_t>(input);
        BOOST_CHECK(result == L"编码");
    }

    //wstring -> u8string
    {
        std::wstring input = L"编码";
        std::u8string result = KIWI_FAST_PLUGIN_UTILITY_NAMESPACE_QUALIFIER code_conversion<char8_t>(input);
        BOOST_CHECK(result == u8"编码");
    }
}

BOOST_AUTO_TEST_SUITE_END()
