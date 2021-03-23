#pragma once

#ifndef UNICODE      /* mingw-w64 gcc 指定编译参数-municode(在CMakeLists中为target_compile_options(locale2 PUBLIC -municode))会定义UNICODE，然后这里利用UNICODE定义_UNICODE */
 #undef _UNICODE
#else
 #ifndef _UNICODE
  #define _UNICODE
 #endif
#endif

#include <tchar.h>

/* 注意还需要指定连接参数-municode(在CMakeLists中为target_link_options(locale2 PUBLIC -municode))这样链接crt0_w.c对应的库，而不是crt0_c.c对应的库 */

#define KIWI_FAST_VERSION_NAMESPACE v1

namespace kiwi_fast{
    inline namespace KIWI_FAST_VERSION_NAMESPACE{}
}

#define KIWI_FAST_OPEN_NAMESPACE namespace kiwi_fast { inline namespace KIWI_FAST_VERSION_NAMESPACE {
#define KIWI_FAST_CLOSE_NAMESPACE }}

#define KIWI_FAST_NAMESPACE kiwi_fast::KIWI_FAST_VERSION_NAMESPACE
#define KIWI_FAST_NAMESPACE_QUALIFIER kiwi_fast::KIWI_FAST_VERSION_NAMESPACE::

#define KIWI_FAST_NAMESPACE_USE using namespace KIWI_FAST_NAMESPACE;

#define KIWI_FAST_ANONYMOUS_NAMESPACE namespace

#define KIWI_FAST_WINDOWS_API

#if defined(__linux__)
#define KIWI_FAST_LINUX
#elif defined(_WIN32)
#define KIWI_FAST_WINDOWS
#endif
