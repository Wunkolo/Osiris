#include "LogModule.hpp"

#include <Ausar/Ausar.hpp>
#include <Utils/Utils.hpp>
#include <stdarg.h>

TODO("I'm currently guessing on the severity and type parameters, they'll need proper flags/enums created eventually")
void LogMessage(const char* SourceFile, uint32_t SourceLine, uint32_t Severity, uint32_t Type, const char* Message, ...)
{
    // construct formatted message
    va_list ArgList;
    va_start(ArgList, Message);
    intmax_t BufferSize = _vscprintf(Message, ArgList) + 1;
    char* Buffer = new char[BufferSize];
    vsprintf_s(Buffer, BufferSize, Message, ArgList);
    va_end(ArgList);

    LOG << SourceFile << ":" << std::dec << SourceLine << " - " << Buffer << std::endl;
    delete[] Buffer;
}

LogModule::LogModule()
{
    // empty zip password
    Util::Process::GetModuleBase()(0x3344630).Write<uint8_t>(0);

    // EnableReleaseLogShim and hook the main ingame logger
    TODO("Break this out into an external config option");
    bool RedirectGameLogs = false;

    Util::Process::GetModuleBase()(0x58938C8).Write<uint8_t>(RedirectGameLogs);
    Util::Process::GetModuleBase()(0x58938D0).Write<void*>(LogMessage);

    LOG << "Log Module Loaded" << std::endl;
}

LogModule::~LogModule()
{
}

void LogModule::Tick(const std::chrono::high_resolution_clock::duration & DeltaTime)
{
    static std::chrono::high_resolution_clock::duration Timer
        = std::chrono::high_resolution_clock::duration::zero();

    Timer += DeltaTime;
}