#pragma once

#include <stdint.h>

#include "Pointer.hpp"

namespace Util
{
    namespace Thread
    {
        uintmax_t GetCurrentThreadId();

        Pointer GetThreadLocalStorage(size_t Index);
    }
}