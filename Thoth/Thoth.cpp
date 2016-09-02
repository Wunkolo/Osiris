#include <iostream>
#include <iomanip>
#include <thread>

#include <conio.h> // _getch()
#include <cctype> //isprint
#include <Windows.h>

#include "Thoth.hpp"

//#include "Modules/ThothModules.hpp"

#include <Utils/Pointer.hpp>

Thoth::Thoth()
{
    Console::SetTextColor(Console::Color::Info);
    std::cout << "Thoth [" << __DATE__ << " : " << __TIME__ << ']' << std::endl;
    Console::SetTextColor(Console::Color::Cyan | Console::Color::Bright);
    std::cout << "\t\xC4Wunkolo (Wunkolo@gmail.com)\n";
    std::cout << std::string(Console::GetWidth() - 1, '\xC4') << std::endl;
    Console::SetTextColor(Console::Color::Info);

    std::string Path;
    Path.resize(MAX_PATH);

    GetModuleFileNameA(nullptr, &Path[0], MAX_PATH);
    std::cout << "Process Module Path: " << Path << std::endl;

    std::cout << std::hex << std::uppercase << std::setfill('0');
    std::cout << "Thoth Thread ID: 0x" << GetCurrentThreadId() << std::endl;
    std::cout << "Base: 0x" << Util::Pointer::Base() << std::endl;

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