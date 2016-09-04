#include "Thread.hpp"

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
#ifdef _WIN64
            return Pointer(__readgsdword(0x1480 + Index * 8));
#else
            return Pointer(__readfsdword(0x18))(0xE10).Read<uintptr_t>();
#endif
        }
    }
}