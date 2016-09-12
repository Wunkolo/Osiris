#pragma once

#include <stdint.h>

#include "Pointer.hpp"
#include "Process.hpp"

namespace Util
{
    namespace Thread
    {
        uintmax_t GetCurrentThreadId();

        bool GetThreadLocalStorage(uint32_t ThreadID, Pointer &TLSArray);

        // Return false to stop iteration
        typedef bool(*ThreadCallback)(
            uint32_t ThreadID
            );

        // Iterates all threads of the current process
        void IterateThreads(
            ThreadCallback ThreadProc,
            uint32_t ProcessID = Process::GetProcessID()
        );
    }
}