#include <iostream>
#include <iomanip>

#include <Windows.h>
#include <ShlObj.h>

#include <conio.h> // _getch()

#include "Osiris.hpp"

//#include "Modules/OsirisModules.hpp"

#include <Utils/Utils.hpp>

Osiris::Osiris()
{
    wchar_t Buffer[MAX_PATH] = { 0 };
    SHGetSpecialFolderPathW(nullptr, Buffer, CSIDL_PROFILE, false);

    std::wstring UserFolder(Buffer);
    UserFolder += L"\\AppData\\Local\\Packages\\Microsoft.Halo5Forge_8wekyb3d8bbwe\\LocalState\\Log.txt";

    Util::Log::Instance()->SetFile(UserFolder);

    LOG << "Osiris" << "---- ";
    LOG << '[' << __DATE__ << " : " << __TIME__ << ']' << std::endl;
    LOG << "\t-Wunkolo (Wunkolo@gmail.com)\n";
    LOG << std::wstring(80, '-') << std::endl;

    LOG << std::hex << std::uppercase << std::setfill(L'0');
    LOG << "Process Base: 0x" << Util::Process::Base() << std::endl;
    LOG << "Osiris Thread ID: 0x" << Util::Thread::GetCurrentThreadId() << std::endl;
    LOG << "Osiris Base: 0x" << Util::Process::GetModuleBase("Osiris.dll") << std::endl;

    // Push Commands
    //PushModule<Research>("research");
    //PushModule<Player>("player");
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