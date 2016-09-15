#pragma once

#include "Pointer.hpp"

namespace Util
{
    namespace Hook
    {
        bool WriteCall(
            Pointer Address,
            Pointer Destination
        );

        bool WriteJump(
            Pointer Address,
            Pointer Destination
        );
    }
}