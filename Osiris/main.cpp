#include <stddef.h>
#include <stdint.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <functional>

#include <Windows.h>

#include <Console\Console.hpp>

#include "Osiris.hpp"

int32_t __stdcall StructuredExceptionPrinter(
    uint32_t ErrorCode,
    const EXCEPTION_POINTERS* const ExceptionInfo
);

uint32_t __stdcall OsirisThread(void*)
{
    __try
    {
        []()
        {
            Console::AllocateConsole("Osiris");
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
        }();
    }
    __except( StructuredExceptionPrinter(GetExceptionCode(), GetExceptionInformation()) )
    {
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
            MessageBox(nullptr, "Unable to disable thread library calls", "Osiris", MB_OK);
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

int32_t __stdcall StructuredExceptionPrinter(uint32_t ErrorCode, const EXCEPTION_POINTERS* const ExceptionInfo)
{
    std::cout << std::endl << "Exception: [0x" << std::uppercase << std::hex << ExceptionInfo->ExceptionRecord->ExceptionCode << ']';

    std::stringstream ErrorMessage;
    ErrorMessage << std::hex << std::setfill('0');

    switch( ErrorCode )
    {
    default:
    {
        ErrorMessage << "Unknown";
        break;
    }
    case EXCEPTION_ACCESS_VIOLATION:
    {
        ErrorMessage << "EXCEPTION_ACCESS_VIOLATION [";

        const char* const MemOperation[2] = { "Reading","Writing" };
        ErrorMessage << MemOperation[ExceptionInfo->ExceptionRecord->ExceptionInformation[0] & 1];

        ErrorMessage << " to 0x"
            << std::uppercase << std::setw(sizeof(uintptr_t) << 1)
            << ExceptionInfo->ExceptionRecord->ExceptionInformation[1] << ']';

        break;
    }
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    {
        ErrorMessage << "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        break;
    }
    case EXCEPTION_BREAKPOINT:
    {
        ErrorMessage << "EXCEPTION_BREAKPOINT";
        break;
    }
    case EXCEPTION_DATATYPE_MISALIGNMENT:
    {
        ErrorMessage << "EXCEPTION_DATATYPE_MISALIGNMENT";
        break;
    }
    case EXCEPTION_FLT_DENORMAL_OPERAND:
    {
        ErrorMessage << "EXCEPTION_FLT_DENORMAL_OPERAND";
        break;
    }
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    {
        ErrorMessage << "EXCEPTION_FLT_DIVIDE_BY_ZERO";
        break;
    }
    case EXCEPTION_FLT_INEXACT_RESULT:
    {
        ErrorMessage << "EXCEPTION_FLT_INEXACT_RESULT";
        break;
    }
    case EXCEPTION_FLT_INVALID_OPERATION:
    {
        ErrorMessage << "EXCEPTION_FLT_INVALID_OPERATION";
        break;
    }
    case EXCEPTION_FLT_OVERFLOW:
    {
        ErrorMessage << "EXCEPTION_FLT_OVERFLOW";
        break;
    }
    case EXCEPTION_FLT_STACK_CHECK:
    {
        ErrorMessage << "EXCEPTION_FLT_STACK_CHECK";
        break;
    }
    case EXCEPTION_FLT_UNDERFLOW:
    {
        ErrorMessage << "EXCEPTION_FLT_UNDERFLOW";
        break;
    }
    case EXCEPTION_ILLEGAL_INSTRUCTION:
    {
        ErrorMessage << "EXCEPTION_ILLEGAL_INSTRUCTION";
        break;
    }
    case EXCEPTION_IN_PAGE_ERROR:
    {
        ErrorMessage << "EXCEPTION_IN_PAGE_ERROR";
        break;
    }
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
    {
        ErrorMessage << "EXCEPTION_INT_DIVIDE_BY_ZERO";
        break;
    }
    case EXCEPTION_INT_OVERFLOW:
    {
        ErrorMessage << "EXCEPTION_INT_OVERFLOW";
        break;
    }
    case EXCEPTION_INVALID_DISPOSITION:
    {
        ErrorMessage << "EXCEPTION_INVALID_DISPOSITION";
        break;
    }
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    {
        ErrorMessage << "EXCEPTION_NONCONTINUABLE_EXCEPTION";
        break;
    }
    case EXCEPTION_PRIV_INSTRUCTION:
    {
        ErrorMessage << "EXCEPTION_PRIV_INSTRUCTION";
        break;
    }
    case EXCEPTION_SINGLE_STEP:
    {
        ErrorMessage << "EXCEPTION_SINGLE_STEP";
        break;
    }
    case EXCEPTION_STACK_OVERFLOW:
    {
        ErrorMessage << "EXCEPTION_STACK_OVERFLOW";
        break;
    }
    }

    // Format exception string
    std::cout << std::nouppercase << " - " << ErrorMessage.str() << std::endl;

    return EXCEPTION_CONTINUE_SEARCH;
}