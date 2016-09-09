#include "Hook.hpp"

#include <Windows.h>
#include <array>
#include <algorithm>

namespace Util
{
    namespace Hook
    {
        void WriteCall(Pointer Address, Pointer Destination)
        {
            std::array<uint8_t, 5> Opcode = { 0xE8 ,0 ,0 ,0 ,0 };
            uint32_t Distance = static_cast<uint32_t>(
                Destination(Address)(Opcode.size())
                );

            uint32_t MemoryAccess, Temp;

            if( !VirtualProtect(
                Address.Point<void>(),
                5,
                PAGE_READWRITE,
                reinterpret_cast<DWORD*>(&MemoryAccess)
            ) )
            {
                // Error unprotecting memory
            }

            // Write Offset to opcode
            std::copy_n(
                reinterpret_cast<const uint8_t*>(&Distance),
                4,
                Opcode.data() + 1
            );

            // Write Opcode
            Address.Write(Opcode);

            VirtualProtect(
                Address.Point<void>(),
                5,
                MemoryAccess,
                reinterpret_cast<DWORD*>(&Temp)
            );
        }

        void WriteJump(Pointer Address, Pointer Destination)
        {
            std::array<uint8_t, 5> Opcode = { 0xE9 ,0 ,0 ,0 ,0 };
            uint32_t Distance = static_cast<uint32_t>(
                Destination(Address)(Opcode.size())
                );

            uint32_t MemoryAccess, Temp;

            if( !VirtualProtect(
                Address.Point<void>(),
                5,
                PAGE_READWRITE,
                reinterpret_cast<DWORD*>(&MemoryAccess)
            ) )
            {
                // Error unprotecting memory
            }

            // Write Offset to opcode
            std::copy_n(
                reinterpret_cast<const uint8_t*>(&Distance),
                4,
                Opcode.data() + 1
            );

            // Write Opcode
            Address.Write(Opcode);

            VirtualProtect(
                Address.Point<void>(),
                5,
                MemoryAccess,
                reinterpret_cast<DWORD*>(&Temp)
            );
        }
    }
}