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
    Console::SetTextColor(Console::Info);
    std::cout << "Thoth [" << __DATE__ << " : " << __TIME__ << ']' << std::endl;
    Console::SetTextColor(Console::Cyan | Console::Bright);
    std::cout << "\t\xC4Wunkolo (Wunkolo@gmail.com)\n";
    std::cout << std::string(Console::GetWidth() - 1, '\xC4') << std::endl;
    Console::SetTextColor(Console::Info);

    char *Path = new char[MAX_PATH]();

    GetModuleFileNameA(nullptr, Path, MAX_PATH);
    std::cout << "Process Module Path: " << Path << std::endl;
    delete[] Path;

    std::cout << std::hex << std::uppercase << std::setfill('0');
    std::cout << "Thoth Thread ID: 0x" << GetCurrentThreadId() << std::endl;
    std::cout << "Base: 0x" << Util::Pointer::Base() << std::endl;

    // Push Commands
    //PushModule<Research>("research");
    //PushModule<Player>("player");
    Terminal.PrintLine();
}

Thoth::~Thoth()
{
}

void Thoth::Tick(const std::chrono::high_resolution_clock::duration &DeltaTime)
{
    if( _kbhit() )
    {
        Terminal.HandleInput(_getch());
        Terminal.PrintLine();
    }
    for( std::pair<std::string, std::shared_ptr<ThothModule>> Command : Commands )
    {
        if( Command.second )
        {
            Command.second->Tick(DeltaTime);
        }
    }
}