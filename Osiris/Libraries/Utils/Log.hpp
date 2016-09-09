#pragma once

#include "Singleton.hpp"
#include <fstream>
#include <memory>

#define LOG (*(Util::Log::Instance().get()->GetStream().get()))

namespace Util
{
    class Log : public Singleton<Log>
    {
    public:
        Log();
        ~Log();

        bool SetFile(const std::wstring &FileName);

        std::shared_ptr<std::wofstream> GetStream() const;

    private:
        std::shared_ptr<std::wofstream> Stream;
    };
}