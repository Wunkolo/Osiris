#pragma once
#include "Pointer.hpp"

namespace Util
{
    namespace Process
    {
        int32_t GetProcessID();

        Pointer GetModuleBase();
        Pointer GetModuleBase(const char* ModuleName);

        inline static Pointer Base()
        {
            const static Pointer ProgramBase(GetModuleBase());
            return ProgramBase;
        }
    }
}