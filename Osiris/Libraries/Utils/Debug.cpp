#include "Debug.hpp"

#include <windows.h>
#include <iphlpapi.h>
#include <vector>
#include <cstdint>
#include <sstream>

namespace Util
{
    namespace Debug
    {
        std::string GetStackTraceString(uint32_t FramesToSkip, uint32_t FramesToCapture)
        {
            std::stringstream StackString;
            std::vector<uint32_t> BackTrace(FramesToCapture);
            uint16_t CapturedFrames = CaptureStackBackTrace(
                FramesToSkip + 1,
                FramesToCapture,
                reinterpret_cast<void**>(BackTrace.data()),
                nullptr
            );

            for( uint16_t i = CapturedFrames - 1; i >= 0; i-- )
            {
                if( i == CapturedFrames - 1 )
                {
                    StackString << "0x" << std::uppercase << std::hex << BackTrace[i];
                }
                else
                {
                    StackString << "->" << "0x" << std::uppercase << std::hex << BackTrace[i];
                }
            }
            return StackString.str();
        }
    }
}