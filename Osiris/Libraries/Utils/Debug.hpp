#pragma once
#include <string>

namespace Util
{
    namespace Debug
    {
        std::string GetStackTraceString(uint32_t FramesToSkip, uint32_t FramesToCapture);
    }
}