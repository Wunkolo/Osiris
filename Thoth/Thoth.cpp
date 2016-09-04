#include <iostream>
#include <iomanip>

#include <Windows.h>

#include <conio.h> // _getch()

#include "Thoth.hpp"

//#include "Modules/ThothModules.hpp"

#include <Utils/Utils.hpp>

Thoth::Thoth()
{
    Console::SetTextColor(Console::Color::Info);
    std::cout << "Thoth" << "\xC4\xC4\xC4\xC2 ";
    std::cout << '[' << __DATE__ << " : " << __TIME__ << ']' << std::endl;
    Console::SetTextColor(Console::Color::Cyan | Console::Color::Bright);
    std::cout << "\t\xC0Wunkolo (Wunkolo@gmail.com)\n";
    Console::SetTextColor(Console::Color::Magenta | Console::Color::Bright);
    std::cout << std::string(Console::GetWidth() - 1, '\xC4') << std::endl;
    Console::SetTextColor(Console::Color::Info);

    std::string Path;
    Path.resize(MAX_PATH + 1, '\x0');

    GetModuleFileNameA(nullptr, &Path[0], MAX_PATH);
    std::cout << "Process Module Path:\n\t" << Path.c_str() << std::endl;
    std::cout << std::hex << std::uppercase << std::setfill('0');
    std::cout << "Process Base: 0x" << Util::Process::Base() << std::endl;
    std::cout << "Thoth Thread ID: 0x" << GetCurrentThreadId() << std::endl;
    std::cout << "Thoth Thread ID: 0x" << Util::Thread::GetCurrentThreadId() << std::endl;
    std::cout << "Thoth Base: 0x" << Util::Process::GetModuleBase("Thoth.dll") << std::endl;

    Util::Process::IterateModules(
        [](
            const char* Name,
            const char* Path,
            Util::Pointer Base,
            size_t Size
            ) -> bool
    {
        std::cout << Name << std::endl;
        std::cout << Path << std::endl;
        std::cout << std::setw(8) << Base << std::endl;
        std::cout << Size << std::endl;
        return true;
    }
    );

    // Push Commands
    //PushModule<Research>("research");
    //PushModule<Player>("player");
    Console::Console::Instance()->PrintLine();
}

Thoth::~Thoth()
{
}

void Thoth::Tick(const std::chrono::high_resolution_clock::duration &DeltaTime)
{
    if( _kbhit() )
    {
        Console::Console::Instance()->HandleInput(_getch());
        Console::Console::Instance()->PrintLine();
    }
    for( std::pair<std::string, std::shared_ptr<ThothModule>> Command : Commands )
    {
        if( Command.second )
        {
            Command.second->Tick(DeltaTime);
        }
    }
}