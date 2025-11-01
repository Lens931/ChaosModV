#pragma once

#include <windows.h>

template <typename F>
inline void InitOnceSpin(volatile LONG &flag, F &&fn)
{
        if (InterlockedCompareExchange(&flag, 1, 0) == 0)
        {
                fn();
        }
        else
        {
                while (flag != 1)
                {
                        ::Sleep(0);
                }
        }
}
