#include "Log.hpp"

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

        NewStream->open(
            FileName,
            std::wofstream::out | std::wofstream::app);

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
}