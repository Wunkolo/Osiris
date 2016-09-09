#include "Log.hpp"
#include <ctime>
#include <iomanip>

// _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

namespace Util
{
    Log::Log()
        :
        Stream(nullptr)
    {
        // Initial Log file attempt at "Log.txt"
        Stream = std::make_shared<std::wofstream>();
    }

    Log::~Log()
    {
    }

    bool Log::SetFile(const std::wstring &FileName)
    {
        std::shared_ptr<std::wofstream> NewStream;

        NewStream = std::make_shared<std::wofstream>();

        NewStream->open(FileName);

        if( NewStream->is_open() )
        {
            Stream->close();
            Stream = NewStream;
            return true;
        }

        return false;
    }

    std::shared_ptr<std::wofstream> Log::GetStream() const
    {
        return Stream;
    }

    std::shared_ptr<std::wofstream> Log::GetStreamTimeStamp()
    {
        time_t Time = std::time(nullptr);
        tm LocalTime = *std::localtime(&Time);

        (*Stream.get()) << std::put_time(
            &LocalTime, L"[%F %T] : ");

        return GetStream();
    }
}