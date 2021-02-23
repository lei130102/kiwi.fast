#pragma once

#include <iostream>
#include <iomanip>
#include <codecvt>

#include <interface/config.h>

namespace test_code_convert
{
    template<typename T>
    void print_hex(T* p)
    {
        while(*p != 0)
        {
            unsigned char const* pByte = reinterpret_cast<unsigned char const*>(p);
            for(int i = 0; i < sizeof(*p); ++i)
            {
                /* 最好将std::setw等设置到这里，防止下次失效 */
                std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(*(pByte + i)) << ' ';
            }

            ++p;
        }
        std::cout << '\n';
    }
}

