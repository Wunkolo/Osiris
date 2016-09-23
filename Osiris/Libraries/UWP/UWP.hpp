#pragma once
#include <string>

// UWP Helper class
namespace UWP
{
    namespace Current
    {
        std::wstring GetFamilyName();

        std::wstring GetFullName();

        std::wstring GetArchitecture();

        std::wstring GetPublisher();

        std::wstring GetPackagePath();
    }
}