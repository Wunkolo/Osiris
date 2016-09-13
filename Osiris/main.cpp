#include <stddef.h>
#include <stdint.h>
#include <chrono>
#include <Windows.h>
#include <ShlObj.h>

#include "Osiris.hpp"

uint32_t __stdcall OsirisThread(void*)
{
    std::chrono::high_resolution_clock::time_point PrevTime, CurTime;
    CurTime = std::chrono::high_resolution_clock::now();
    while( true )
    {
        PrevTime = CurTime;
        CurTime = std::chrono::high_resolution_clock::now();
        Osiris::Instance()->Tick(
            (CurTime - PrevTime)
        );
    }
    return 0;
}

int32_t __stdcall DllMain(HINSTANCE hDLL, uint32_t Reason, void *Reserved)
{
    switch( Reason )
    {
    case DLL_PROCESS_ATTACH:
    {
        if( !DisableThreadLibraryCalls(hDLL) )
        {
            return false;
        }

        CreateThread(
            nullptr,
            0,
            reinterpret_cast<unsigned long(__stdcall*)(void*)>(&OsirisThread),
            nullptr,
            0,
            nullptr);
    }
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
    {
        return true;
    }
    }

    return false;
}