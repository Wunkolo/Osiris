#pragma once
#include <stdint.h>

namespace Ausar
{
    namespace Threading
    {
#pragma pack(push,1)
        struct ThreadTable
        {
            struct ThreadEntry
            {
                uint8_t Active;
                // Null terminated ascii
                char ThreadName[0x40];
                uint8_t Padding[7];
                // Priority? Thread state?
                uint64_t Unknown48;
                uint64_t ThreadID;
            } Entries[64];

            // returns -1 on error
            uint64_t GetThreadIDByName(const char* Name) const;
        };
#pragma pack(pop)
    }
}