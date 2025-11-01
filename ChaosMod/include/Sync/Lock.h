#pragma once

#include <windows.h>

struct SrwLock
{
        SRWLOCK L = SRWLOCK_INIT;
        void lock()
        {
                AcquireSRWLockExclusive(&L);
        }
        void unlock()
        {
                ReleaseSRWLockExclusive(&L);
        }
};

struct SrwRead
{
        SRWLOCK L = SRWLOCK_INIT;
        void lock()
        {
                AcquireSRWLockShared(&L);
        }
        void unlock()
        {
                ReleaseSRWLockShared(&L);
        }
};

struct AutoLock
{
        SrwLock &r;
        explicit AutoLock(SrwLock &x)
            : r(x)
        {
                r.lock();
        }
        ~AutoLock()
        {
                r.unlock();
        }
};
