#include "Checksum.hpp"

namespace Util
{
    namespace Checksum
    {
        uint16_t Crc16(const void* Data, size_t Length, uint16_t PrevCrc)
        {
            const uint16_t Polynomial = 0x8408;
            uint16_t Crc = ~PrevCrc;
            const uint8_t* Current = reinterpret_cast<const uint8_t*>(Data);
            while( Length-- )
            {
                Crc ^= *Current++;
                for( uint8_t i = 0; i < 8; i++ )
                {
                    Crc = (Crc >> 1) ^ (-static_cast<int16_t>(Crc & 1) & Polynomial);
                }
            }
            return ~Crc;
        }

        uint32_t Crc32(const void* Data, size_t Length, uint32_t PrevCrc)
        {
            const uint32_t Polynomial = 0xEDB88320;
            uint32_t Crc = ~PrevCrc;
            const uint8_t* Current = reinterpret_cast<const uint8_t*>(Data);
            while( Length-- )
            {
                Crc ^= *Current++;
                for( uint8_t i = 0; i < 8; i++ )
                {
                    Crc = (Crc >> 1) ^ (-static_cast<int32_t>(Crc & 1) & Polynomial);
                }
            }

            return ~Crc;
        }

        uint16_t Fletcher16(const void* Data, size_t Length)
        {
            uint16_t sum1, sum2;
            sum1 = sum2 = 0;
            for( size_t i = 0; i < Length; ++i )
            {
                sum1 += static_cast<const uint8_t*>(Data)[i];
                sum1 %= 0xff;
                sum2 += sum1;
                sum2 %= 0xff;
            }
            return (sum2 << 8) | sum1;
        }

        uint16_t Bsd16(const void* Data, size_t Length)
        {
            uint16_t Checksum;
            for( size_t i = 0; i < Length; i++ )
            {
                Checksum = (Checksum >> 1) | ((Checksum & 1) << 15);
                Checksum += static_cast<const uint8_t*>(Data)[i];
                Checksum &= 0xffff;
            }
            return Checksum;
        }

        uint32_t Fnv32(const void* Data, size_t Length)
        {
            const uint32_t OffsetBasis = 2166136261u;
            const uint32_t Prime = 16777619u;

            uint32_t Checksum = OffsetBasis;
            for( size_t i = 0; i < Length; i++ )
            {
                Checksum *= Prime;
                Checksum ^= static_cast<const uint8_t*>(Data)[i];
            }
            return Checksum;
        }

        uint32_t FnvA32(const void* Data, size_t Length)
        {
            const uint32_t OffsetBasis = 2166136261u;
            const uint32_t Prime = 16777619u;

            uint32_t Checksum = OffsetBasis;
            for( size_t i = 0; i < Length; i++ )
            {
                Checksum ^= static_cast<const uint8_t*>(Data)[i];
                Checksum *= Prime;
            }
            return Checksum;
        }

        uint64_t Fnv64(const void* Data, size_t Length)
        {
            const uint64_t OffsetBasis = 14695981039346656037u;
            const uint64_t Prime = 1099511628211u;

            uint64_t Checksum = OffsetBasis;
            for( size_t i = 0; i < Length; i++ )
            {
                Checksum *= Prime;
                Checksum ^= static_cast<const uint8_t*>(Data)[i];
            }
            return Checksum;
        }

        uint64_t FnvA64(const void* Data, size_t Length)
        {
            const uint64_t OffsetBasis = 14695981039346656037u;
            const uint64_t Prime = 1099511628211u;

            uint64_t Checksum = OffsetBasis;
            for( size_t i = 0; i < Length; i++ )
            {
                Checksum ^= static_cast<const uint8_t*>(Data)[i];
                Checksum *= Prime;
            }
            return Checksum;
        }

        uint32_t Murmur32(const void* Data, uint32_t Length, uint32_t Seed)
        {
            const uint32_t C1 = 0xCC9E2D51;
            const uint32_t C2 = 0x1B873593;
            const uint32_t R1 = 15;
            const uint32_t R2 = 13;
            const uint32_t M = 5;
            const uint32_t N = 0xE6546B64;
            uint32_t K1 = 0;

            uint32_t Hash = Seed;
            uint32_t Chunks = Length >> 2;

            // Process all 4 byte chunks
            for( size_t i = K1 = 0; i < Chunks; i++ )
            {
                uint32_t K = static_cast<const uint32_t*>(Data)[i];
                K *= C1;
                K = (K << R1) | (K >> (32 - R1));
                K *= C2;

                Hash ^= K;
                Hash = ((Hash << R2) | (Hash >> (32 - R2)));
                Hash *= M + N;
            }

            // Remaining bytes
            const uint32_t *End = static_cast<const uint32_t*>(Data) + Chunks * 4;

            switch( Length & 3 )
            {
            case 3:
            {
                Hash ^= End[2] << 16;
                break;
            }
            case 2:
            {
                K1 ^= End[1] << 8;
                break;
            }
            case 1:
            {
                K1 ^= End[0];
                K1 = (K1 << R1) | (K1 >> (32 - R1));
                K1 *= C2;
                Hash ^= K1;
                break;
            }
            default:
                break;
            }

            Hash ^= Length;
            Hash ^= (Hash >> 16);
            Hash *= 0x85EBCA6B;
            Hash ^= (Hash >> 13);
            Hash *= 0xC2B2AE35;
            Hash ^= (Hash >> 16);

            return 0;
        }
    }
}