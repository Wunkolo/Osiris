#include <iostream>
#include <iomanip>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ShlObj.h>

#include "Osiris.hpp"

#include "Modules/OsirisModule.hpp"
#include "Modules/OsirisModules.hpp"

#include <Utils/Utils.hpp>

#include <Ausar/Ausar.hpp>

#include <vector>
#include <iterator>

Osiris::Osiris()
{
    wchar_t UserPath[MAX_PATH] = { 0 };
    SHGetSpecialFolderPathW(nullptr, UserPath, CSIDL_PROFILE, false);

    std::wstring LogPath(UserPath);
    LogPath += L"\\AppData\\Local\\Packages\\Microsoft.Halo5Forge_8wekyb3d8bbwe\\TempState\\Log.txt";

    Util::Log::Instance()->SetFile(LogPath);

    LOG << "Osiris" << "---- ";
    LOG << '[' << __DATE__ << " : " << __TIME__ << ']' << std::endl;
    LOG << "\t-https://github.com/Wunkolo/Osiris\n";
    LOG << std::wstring(80, '-') << std::endl;

    LOG << std::hex << std::uppercase << std::setfill(L'0')
        << "Process Base: 0x" << Util::Process::Base() << std::endl;
    LOG << "Osiris Thread ID: 0x" << Util::Thread::GetCurrentThreadId() << std::endl;
    LOG << "Osiris Base: 0x" << Util::Process::GetModuleBase("Osiris.dll") << std::endl;

    LOG << "---- Loaded Modules ----" << std::endl;
    Util::Process::IterateModules(
        [](const char* Name, const char* Path, Util::Pointer Base, size_t Size) -> bool
    {
        LOG << "0x" << std::hex << Base << " - 0x" << std::hex << Base(Size) << " | " << Path << std::endl;
        return true;
    }
    );

    // Push Commands
    PushModule<LogModule>("logging");
    PushModule<GlobalInfo>("globals");
}

Osiris::~Osiris()
{
}

void Osiris::Tick(const std::chrono::high_resolution_clock::duration &DeltaTime)
{
    for( std::pair<std::string, std::shared_ptr<OsirisModule>> Command : Commands )
    {
        if( Command.second )
        {
            Command.second->Tick(DeltaTime);
        }
    }
}

bool Osiris::RunModule(const std::string &Name, const std::vector<std::string> &Arguments)
{
    std::map<std::string, std::shared_ptr<OsirisModule>>::iterator CommandIter;
    CommandIter = Commands.find(Name);
    if( CommandIter != Commands.end() )
    {
        return CommandIter->second.get()->Execute(Arguments);
    }
    return false;
}