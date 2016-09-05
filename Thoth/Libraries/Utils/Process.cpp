#include "Process.hpp"

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

        void IterateModules(ModuleCallback ModuleProc)
        {
            void* hSnapShot = CreateToolhelp32Snapshot(
                TH32CS_SNAPMODULE,
                GetCurrentProcessId()
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
    }
}