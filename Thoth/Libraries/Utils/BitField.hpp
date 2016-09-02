#pragma once
#include <stdint.h>

namespace Util
{
    template<size_t Index, size_t BitCount, typename BitType>
    class BitField
    {
    public:
        BitField() : Field(0)
        {
        }

        BitField(BitType Value) : Field(Value)
        {
        }

        operator BitType() const
        {
            return (Field >> Index) & Mask;
        }

        explicit operator bool() const
        {
            return (Field & (Mask << Index)) != BitType(0);
        }

        BitField& operator++()
        {
            return *this = *this + 1;
        }

        BitField& operator--()
        {
            return *this = *this - 1;
        }

        BitField operator++(int Operand)
        {
            BitType result = *this;
            ++*this;
            return result;
        }

        BitField operator--(int Operand)
        {
            BitType result = *this;
            --*this;
            return result;
        }

        template<class T>
        BitField& operator=(T Value)
        {
            Field = (Field & ~(Mask << Index)) | ((Value & Mask) << Index);
            return *this;
        }

    private:
        BitType Field;
        enum : BitType
        {
            Mask = (BitType(1) << BitCount) - BitType(1)
        };
    };

    template<size_t Index, size_t BitCount>
    using BitField8 = BitField<Index, BitCount, uint8_t>;

    template<size_t Index, size_t BitCount>
    using BitField16 = BitField<Index, BitCount, uint16_t>;

    template<size_t Index, size_t BitCount>
    using BitField32 = BitField<Index, BitCount, uint32_t>;

    template<size_t Index, size_t BitCount>
    using BitField64 = BitField<Index, BitCount, uint64_t>;
}