#pragma once

#include "Pointer.hpp"

namespace Util
{
    namespace Hook
    {
        void WriteCall(
            Pointer Address,
            Pointer Destination
        );

        void WriteJump(
            Pointer Address,
            Pointer Destination
        );
    }
}