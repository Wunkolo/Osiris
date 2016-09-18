#pragma once
#include "Pointer.hpp"

namespace Util
{
    namespace Process
    {
        int32_t GetProcessID();

        Pointer GetModuleBase();
        Pointer GetModuleBase(const char* ModuleName);

        inline Pointer Base()
        {
            return GetModuleBase();
        }

        // Return false to stop iteration
        typedef bool(*ModuleCallback)(
            const char* Name,
            const char* Path,
            Pointer Base,
            size_t Size
            );

        // Iterates all modules of the current process
        void IterateModules(
            ModuleCallback ModuleProc,
            uint32_t ProcessID = GetProcessID()
        );
    }
}