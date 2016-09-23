#include <iostream>
#include <iomanip>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Osiris.hpp"

#include "Modules/OsirisModule.hpp"
#include "Modules/OsirisModules.hpp"

#include <UWP/UWP.hpp>
#include <Utils/Utils.hpp>
#include <Ausar/Ausar.hpp>

#include <vector>
#include <iterator>

Osiris::Osiris()
{
    std::wstring WorkingPath = UWP::Current::Storage::GetTempStatePath();

    Util::Log::Instance()->SetFile(WorkingPath + L"\\Log.txt");

    LOG << "Osiris" << "---- ";
    LOG << '[' << __DATE__ << " : " << __TIME__ << ']' << std::endl;
    LOG << "\t-https://github.com/Wunkolo/Osiris\n";
    LOG << "Working Path: " << WorkingPath << std::endl;
    LOG << std::wstring(80, '-') << std::endl;

    LOG << "Publisher: " << UWP::Current::GetPublisher() << std::endl;
    LOG << "Publisher ID: " << UWP::Current::GetPublisherID() << std::endl;
    LOG << "Publisher Path: " << UWP::Current::Storage::GetPublisherPath() << std::endl;

    LOG << "Package Path: " << UWP::Current::GetPackagePath() << std::endl;
    LOG << "Package Name: " << UWP::Current::GetFullName() << std::endl;
    LOG << "Family Name: " << UWP::Current::GetFamilyName() << std::endl;

    LOG << std::hex << std::uppercase << std::setfill(L'0')
        << "Process Base: 0x" << Util::Process::Base() << std::endl;
    LOG << "Osiris Thread ID: 0x" << Util::Thread::GetCurrentThreadId() << std::endl;
    LOG << "Osiris Base: 0x" << Util::Process::GetModuleBase("Osiris.dll") << std::endl;

    //LOG << "---- Loaded Modules ----" << std::endl;
    //Util::Process::IterateModules(
    //    [](const char* Name, const char* Path, Util::Pointer Base, size_t Size) -> bool
    //{
    //    LOG << "0x" << std::hex << Base << " - 0x" << std::hex << Base(Size) << " | " << Path << std::endl;
    //    return true;
    //}
    //);

    // Push Commands
    PushModule<LogModule>("logging");
    PushModule<GlobalInfo>("globals");
    PushModule<PackageDump>("dump");

    TODO("Config file to enable dumping of files");
    // Uncomment this to enable dumping
    //RunModule("dump");
}

Osiris::~Osiris()
{
}

void Osiris::Tick(const std::chrono::high_resolution_clock::duration &DeltaTime)
{
    for( const std::pair<std::string, std::shared_ptr<OsirisModule>>& Command : Commands )
    {
        if( Command.second )
        {
            Command.second->Tick(DeltaTime);
        }
    }
}

bool Osiris::RunModule(const std::string &Name, const std::vector<std::string> &Arguments)
{
    std::map<std::string, std::shared_ptr<OsirisModule>>::const_iterator CommandIter;
    CommandIter = Commands.find(Name);
    if( CommandIter != Commands.end() )
    {
        return CommandIter->second.get()->Execute(Arguments);
    }
    return false;
}