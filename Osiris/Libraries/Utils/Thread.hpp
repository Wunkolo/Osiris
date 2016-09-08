#pragma once

#include <stdint.h>

#include "Pointer.hpp"
#include "Process.hpp"

namespace Util
{
    namespace Thread
    {
        uintmax_t GetCurrentThreadId();

        Pointer GetThreadLocalStorage(size_t Index);

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