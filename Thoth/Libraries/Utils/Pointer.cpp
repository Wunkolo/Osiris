#include "Pointer.hpp"
#include <Windows.h>
#include <TlHelp32.h> //GetModuleBase
#include <memory>
#include <string>

namespace Util
{
    Pointer Pointer::GetModuleBase()
    {
        static void* Base = nullptr;

        if( Base == nullptr )
        {
            HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
            if( hSnapShot == INVALID_HANDLE_VALUE )
                return nullptr;
            MODULEENTRY32 lpModuleEntry;
            lpModuleEntry.dwSize = sizeof(MODULEENTRY32);
            int bRet = Module32First(hSnapShot, &lpModuleEntry);
            CloseHandle(hSnapShot);
            Base = (bRet != 0) ? (void*)lpModuleEntry.modBaseAddr : nullptr;
        }

        return Pointer(Base);
    }

    Pointer Pointer::GetModuleBase(const char* ModuleName)
    {
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
        if( hSnapShot == INVALID_HANDLE_VALUE )
            return nullptr;

        MODULEENTRY32 lpModuleEntry;
        lpModuleEntry.dwSize = sizeof(MODULEENTRY32);
        BOOL bModule = Module32First(hSnapShot, &lpModuleEntry);
        while( bModule )
        {
            //If module name matches: return it
            if( !std::strcmp(ModuleName, lpModuleEntry.szModule) )
            {
                CloseHandle(hSnapShot);
                return (void*)lpModuleEntry.modBaseAddr;
            }
            bModule = Module32Next(hSnapShot, &lpModuleEntry);
        }
        CloseHandle(hSnapShot);
        return nullptr;
    }
}