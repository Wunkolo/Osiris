#include "Process.hpp"

#include <Windows.h>
#include <TlHelp32.h> //GetModuleBase

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

                MODULEENTRY32 lpModuleEntry = { 0 };
                lpModuleEntry.dwSize = sizeof(MODULEENTRY32);
                int32_t bRet = Module32First(hSnapShot, &lpModuleEntry);
                CloseHandle(hSnapShot);
                CacheBase =
                    (bRet != 0) ?
                    reinterpret_cast<void*>(lpModuleEntry.modBaseAddr)
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

            MODULEENTRY32 lpModuleEntry = { 0 };
            lpModuleEntry.dwSize = sizeof(MODULEENTRY32);
            int32_t bModule = Module32First(hSnapShot, &lpModuleEntry);
            while( bModule )
            {
                //If module name matches: return it
                if( !std::strcmp(ModuleName, lpModuleEntry.szModule) )
                {
                    CloseHandle(hSnapShot);
                    return reinterpret_cast<void*>(lpModuleEntry.modBaseAddr);
                }
                bModule = Module32Next(hSnapShot, &lpModuleEntry);
            }
            CloseHandle(hSnapShot);
            return nullptr;
        }
    }
}