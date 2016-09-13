#include "Thread.hpp"

#include <Windows.h>
#include <TlHelp32.h>

#include <winnt.h>

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

        bool GetThreadLocalStorage(uint32_t ThreadID, size_t TlsSlot, Pointer &TLS)
        {
            if( TlsSlot > 1088 ) // Max TLS slots
            {
                return false;
            }

            HANDLE ThreadHandle = OpenThread(
                THREAD_ALL_ACCESS,
                false,
                static_cast<DWORD>(ThreadID)
            );

            if( ThreadHandle == nullptr )
            {
                return false;
            }

            typedef NTSTATUS(WINAPI *InfoThreadProc)(HANDLE, LONG, PVOID, ULONG, PULONG);

            InfoThreadProc NtQueryInformationThread =
                reinterpret_cast<InfoThreadProc>(
                    GetProcAddress(
                        GetModuleHandleA("ntdll.dll"),
                        "NtQueryInformationThread"
                    )
                    );

            if( NtQueryInformationThread == nullptr )
            {
                return false;
            }

            struct THREAD_BASIC_INFORMATION
            {
                NTSTATUS ExitStatus;
                PVOID TebBaseAddress;
                struct CLIENT_ID
                {
                    HANDLE UniqueProcess;
                    HANDLE UniqueThread;
                } ClientId;
                KAFFINITY AffinityMask;
                LONG Priority;
                LONG BasePriority;
            } ThreadInfo = { 0 };

            NTSTATUS ntStatus = NtQueryInformationThread(
                ThreadHandle,
                0,
                &ThreadInfo,
                sizeof(THREAD_BASIC_INFORMATION),
                nullptr
            );

            CloseHandle(ThreadHandle);

            if( ntStatus != 0 )
            {
                return false;
            }

            TLS = Pointer(ThreadInfo.TebBaseAddress)[0x58][0];

            return true;
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