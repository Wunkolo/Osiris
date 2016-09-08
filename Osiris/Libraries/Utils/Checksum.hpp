#pragma once

#include <stdint.h>

namespace Util
{
    namespace Checksum
    {
        uint16_t Crc16(const void* Data, size_t Length, uint16_t PrevCrc = 0);
        uint32_t Crc32(const void* Data, size_t Length, uint32_t PrevCrc = 0);

        uint16_t Fletcher16(const void* Data, size_t Length);

        uint16_t Bsd16(const void* Data, size_t Length);

        uint32_t Fnv32(const void* Data, size_t Length);
        uint32_t FnvA32(const void* Data, size_t Length);

        uint64_t Fnv64(const void* Data, size_t Length);
        uint64_t FnvA64(const void* Data, size_t Length);

        uint32_t Murmur32(const void* Data, uint32_t Length, uint32_t Seed = 0);
    }
}