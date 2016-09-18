#pragma once
#include <stdint.h>
#include <algorithm>

namespace Util
{
    class Pointer
    {
    public:
        Pointer()
            :
            Address(0)
        {
        };

        Pointer(void* Pointer)
            :
            Address(
                reinterpret_cast<std::uintptr_t>(Pointer)
            )
        {
        }

        Pointer(std::uintptr_t Address) : Address(Address)
        {
        }

        // Accessors
        inline std::uintptr_t GetAddress() const
        {
            return Address;
        }

        inline void SetAddress(const std::uintptr_t Address)
        {
            this->Address = Address;
        }

        // Operators

        inline Pointer operator[](const ptrdiff_t Offset) const
        {
            return Pointer(Pointer(Address + Offset).Read<std::uintptr_t>());
        }

        inline Pointer operator()(const ptrdiff_t Offset) const
        {
            return Pointer(Address + Offset);
        }

        inline Pointer operator+ (std::ptrdiff_t Offset) const
        {
            return Pointer(Address + Offset);
        }

        inline Pointer operator- (std::ptrdiff_t Offset) const
        {
            return Pointer(Address - Offset);
        }

		bool operator== (const std::nullptr_t value) const
		{
			return reinterpret_cast<void*>(this->Address) == nullptr;
		}

		bool operator!= (const std::nullptr_t value) const
		{
			return !(this == nullptr);
		}

        // Casting

        inline operator std::uintptr_t() const
        {
            return Address;
        }

        template <typename T>
        inline T* Point() const
        {
            return reinterpret_cast<T*>(Address);
        }

        // Templated Read/Write

        template <typename T>
        inline T& Get() const
        {
            return *Point<T>();
        }

        template <typename T>
        inline const T& Read() const
        {
            return Get<T>();
        }

        inline void Read(void *Destination, size_t Count) const
        {
            std::copy_n(
                Point<uint8_t>(),
                Count,
                reinterpret_cast<uint8_t*>(Destination)
            );
        }

        template <typename T>
        inline void Write(const T& Data) const
        {
            Get<T>() = Data;
        }

        inline void Write(const void *Data, size_t Size) const
        {
            std::copy_n(
                reinterpret_cast<const uint8_t*>(Data),
                Size,
                Point<uint8_t>()
            );
        }

        template <typename T>
        void Fill(const T& Data, const std::size_t Count)
        {
            std::fill_n(Point<T>(), Count, Data);
        }

    private:
        std::uintptr_t Address;
    };
}