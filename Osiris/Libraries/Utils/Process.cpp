#include "Process.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

namespace Util
{
    namespace Process
    {
        int32_t GetProcessID()
        {
            return GetCurrentProcessId();
        }

        Pointer GetModuleBase()
        {
            // Cache base pointer
            static void* CacheBase = nullptr;

            if( CacheBase == nullptr )
            {
                void* hSnapShot = CreateToolhelp32Snapshot(
                    TH32CS_SNAPMODULE,
                    GetCurrentProcessId()
                );

                if( hSnapShot == INVALID_HANDLE_VALUE )
                {
                    return nullptr;
                }

                MODULEENTRY32 ModuleEntry = { 0 };
                ModuleEntry.dwSize = sizeof(MODULEENTRY32);
                int32_t Ret = Module32First(hSnapShot, &ModuleEntry);
                CloseHandle(hSnapShot);
                CacheBase =
                    (Ret != 0) ?
                    reinterpret_cast<void*>(ModuleEntry.modBaseAddr)
                    : nullptr;
            }

            return Pointer(CacheBase);
        }

        Pointer GetModuleBase(const char* ModuleName)
        {
            void* hSnapShot = CreateToolhelp32Snapshot(
                TH32CS_SNAPMODULE,
                GetCurrentProcessId()
            );

            if( hSnapShot == INVALID_HANDLE_VALUE )
            {
                return nullptr;
            }

            MODULEENTRY32 ModuleEntry = { 0 };
            ModuleEntry.dwSize = sizeof(MODULEENTRY32);
            int32_t Ret = Module32First(hSnapShot, &ModuleEntry);
            while( Ret )
            {
                //If module name matches: return it
                if( !std::strcmp(ModuleName, ModuleEntry.szModule) )
                {
                    CloseHandle(hSnapShot);
                    return reinterpret_cast<void*>(ModuleEntry.modBaseAddr);
                }
                Ret = Module32Next(hSnapShot, &ModuleEntry);
            }
            CloseHandle(hSnapShot);
            return nullptr;
        }

        void IterateModules(ModuleCallback ModuleProc, uint32_t ProcessID)
        {
            void* hSnapShot = CreateToolhelp32Snapshot(
                TH32CS_SNAPMODULE,
                ProcessID
            );

            if( hSnapShot == INVALID_HANDLE_VALUE )
            {
                return;
            }

            MODULEENTRY32 ModuleEntry = { 0 };
            ModuleEntry.dwSize = sizeof(MODULEENTRY32);
            Module32First(hSnapShot, &ModuleEntry);
            do
            {
                bool Continue = ModuleProc(
                    ModuleEntry.szModule,
                    ModuleEntry.szExePath,
                    Pointer(ModuleEntry.modBaseAddr),
                    ModuleEntry.modBaseSize
                );
                if( Continue == false )
                {
                    break;
                }
            } while( Module32Next(hSnapShot, &ModuleEntry) );

            CloseHandle(hSnapShot);
            return;
        }

#define READABLE (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY)
#define PROTECTED (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS)
        void IterateReadableMemory(MemoryCallback MemoryProc)
        {
            MEMORY_BASIC_INFORMATION MemInfo;

            for( uint8_t *i = nullptr;
                (VirtualQuery(i, &MemInfo, sizeof(MEMORY_BASIC_INFORMATION)))
                == sizeof(MEMORY_BASIC_INFORMATION);
                i += MemInfo.RegionSize )
            {
                if( (MemInfo.Type & MEM_PRIVATE) == 0 ) // Not private, skip
                {
                    continue;
                }

                if( (MemInfo.State & (MEM_COMMIT)) == 0 ) // Not committed, skip
                {
                    continue;
                }

                if( (MemInfo.Protect & PROTECTED) ) // Protected, skip
                {
                    continue;
                }

                if( !(MemInfo.Protect & READABLE) ) // Not readable, skip
                {
                    continue;
                }

                if( MemoryProc(Pointer(i), MemInfo.RegionSize) == false )
                {
                    break;
                }
            }
        }
    }
}