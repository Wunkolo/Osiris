#include "Thread.hpp"

#include <Windows.h>
#include <TlHelp32.h>

namespace Util
{
    namespace Thread
    {
        uintmax_t GetCurrentThreadId()
        {
#ifdef _WIN64
            /*
            mov         rax,qword ptr gs:[30h]
            mov         eax,dword ptr [rax+48h]
            */
            return Pointer(__readgsqword(0x30))(0x48).Read<uint64_t>();
#else
            /*
            mov         eax,dword ptr fs:[18h]
            mov         eax,dword ptr [eax+24h]
            */
            return Pointer(__readfsdword(0x18))(0x24).Read<uint32_t>();
#endif
        }

        Pointer GetThreadLocalStorage(size_t Index)
        {
            //#ifdef _WIN64
            //            return Pointer(
            //                __readgsqword(0x1480 + static_cast<uint32_t>(Index) * 8)
            //            );
            //#else
            //            return Pointer(
            //                __readfsdword(0x18)
            //            )(0xE10 + static_cast<uint32_t>(Index) * 4).Read<uintptr_t>();
            //#endif
            return TlsGetValue(static_cast<uint32_t>(Index));
        }

        void IterateThreads(ThreadCallback ThreadProc, uint32_t ProcessID)
        {
            void* hSnapShot = CreateToolhelp32Snapshot(
                TH32CS_SNAPTHREAD,
                ProcessID
            );

            if( hSnapShot == INVALID_HANDLE_VALUE )
            {
                return;
            }

            THREADENTRY32 ThreadEntry = { 0 };
            ThreadEntry.dwSize = sizeof(THREADENTRY32);
            Thread32First(hSnapShot, &ThreadEntry);
            do
            {
                if( ThreadEntry.th32OwnerProcessID == ProcessID )
                {
                    bool Continue = ThreadProc(
                        ThreadEntry.th32ThreadID
                    );
                    if( Continue == false )
                    {
                        break;
                    }
                }
            } while( Thread32Next(hSnapShot, &ThreadEntry) );

            CloseHandle(hSnapShot);
            return;
        }
    }
}