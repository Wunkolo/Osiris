#include "Console.hpp"

#include <Windows.h>
#include <conio.h> // _getch()
#pragma warning(disable:4996)

#include <io.h>

#include <cctype> //isgraph

#include <algorithm>

#define ps1beg "\xAF["
#define ps1end "]\xAE"

namespace Console
{
    Console::Console()
        :
        CurArg(0)
    {
        ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleOutputCP(437);

        CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo{};
        if( GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleInfo) )
        {
            ConsoleWidth = ConsoleInfo.dwSize.X;
        }

        SetTextColor(Color::Info);
        CurCommand.push_back("");

        // Meta commands pushed with null deleter so console doesn't delete itself.
        auto NullDeleter = [](Console*) {};

        PushCommand("help", std::shared_ptr<Console>(this, NullDeleter));
        PushCommand("history", std::shared_ptr<Console>(this, NullDeleter));
        PushCommand("quit", std::shared_ptr<Console>(this, NullDeleter));
    }

    Console::~Console()
    {
        Commands.clear();
    }

    void Console::HandleInput(uint32_t KeyCode)
    {
        if( std::isgraph(KeyCode) )
        {
            if( CurCommand.empty() )
            {
                CurCommand.push_back("");
            }
            CurCommand.back().push_back(static_cast<uint8_t>(KeyCode));

            // Suggest
            Suggestion.clear();
            if( CurArg == 0 )
            {
                // Find closest matching command

                std::vector<std::string> Suggestions;

                for( auto Cmd : Commands )
                {
                    if( !CurCommand[0].compare(0, CurCommand[0].length(), (Cmd.first), 0, CurCommand[0].length()) )
                    {
                        Suggestions.push_back((Cmd.first));
                    }
                }

                if( Suggestions.size() )
                {
                    // Return first match
                    Suggestion = Suggestions.front();
                }

                Suggestions.clear();
            }
            else
            {
                if( Commands.count(CurCommand.front()) )
                {
                    Suggestion = Commands[CurCommand.front()]->Suggest(CurCommand);
                }
            }
        }
        else if( KeyCode == ' ' ) // Space
        {
            if( !CurCommand.empty() )
            {
                if( !CurCommand.back().empty() )
                {
                    //Go to next argument;
                    CurArg++;
                    CurCommand.push_back("");
                }
                if( Commands.count(CurCommand.front()) )
                {
                    Suggestion = Commands[CurCommand.front()]->Suggest(CurCommand);
                }
            }
        }
        else if( KeyCode == '\b' ) // Backspace
        {
            if( !CurCommand.empty() )
            {
                // Remove character from current command
                if( !CurCommand.back().empty() )
                {
                    CurCommand.back().pop_back();
                }
                // Current argument is empty.
                // Go back an argument
                if( CurCommand.back().empty() )
                {
                    CurCommand.pop_back();
                    !CurArg || CurArg--;
                }
                // Suggest
                Suggestion.clear();
                if( CurArg == 0 && !CurCommand.empty() )
                {
                    // Find closest matching command

                    std::vector<std::string> Suggestions;

                    for( auto Cmd : Commands )
                    {
                        if( !CurCommand[0].compare(0, CurCommand[0].length(), (Cmd.first), 0, CurCommand[0].length()) )
                        {
                            Suggestions.push_back((Cmd.first));
                        }
                    }

                    if( Suggestions.size() )
                    {
                        // Return first match
                        Suggestion = Suggestions.front();
                    }

                    Suggestions.clear();
                }
                else
                {
                    if( !CurCommand.empty() && Commands.count(CurCommand.front()) )
                    {
                        Suggestion = Commands[CurCommand.front()]->Suggest(CurCommand);
                    }
                }
            }
        }
        else if( KeyCode == '\r' ) // Enter
        {
            SetTextColor(Color::Info);
            // Clear previous suggestion
            if( !Suggestion.empty() )
            {
                size_t i;
                for( i = 0; i < Suggestion.length(); i++ )
                {
                    std::cout << ' ';
                }
                for( i = 0; i < Suggestion.length() + (CurCommand.size() > 1 ? 1 : 0); i++ )
                {
                    std::cout << '\b';
                }
            }

            std::cout << ps1end << std::endl;
            if( !CurCommand.empty() )
            {
                if( CurCommand.back().empty() )
                {
                    CurCommand.pop_back();
                }
                // Run command
                if( Commands.count(CurCommand[0]) && Commands[CurCommand[0]] )
                {
                    if( !Commands[CurCommand[0]]->Run(CurCommand) )
                    {
                        // Error running command;
                        SetTextColor(Color::Error);
                        std::cout << "Invalid Usage: " << CurCommand[0] << std::endl;
                        SetTextColor(Color::Info);
                        std::cout << Commands[CurCommand[0]]->Info() << std::endl;
                    }
                }
                else
                {
                    SetTextColor(Color::Error);
                    std::cout << "Unknown Command: " << CurCommand[0] << std::endl;
                }
            }

            SetTextColor(Color::Info);
            if( !CurCommand.empty() )
            {
                PrevCommands.push_back(CurCommand);
                PrevCommand = PrevCommands.end();
            }
            CurArg = 0;
            CurCommand.clear();
        }
        else if( KeyCode == '\t' ) // Tab
        {
            // Get suggestion
            if( !Suggestion.empty() && !CurCommand.empty() )
            {
                CurCommand.back() = Suggestion;
                Suggestion.clear();
                CurArg++;
                CurCommand.push_back("");
                Suggestion = Commands[CurCommand.front()]->Suggest(CurCommand);
            }
        }
        else if( KeyCode == 22 ) // Ctrl+v
        {
            // Paste in clipbard
            if( OpenClipboard(nullptr) )
            {
                std::string Clipboard(reinterpret_cast<char*>(GetClipboardData(CF_TEXT)));
                CloseClipboard();
                for( char CurChar : Clipboard )
                {
                    HandleInput(CurChar);
                }
            }
        }
        else if( KeyCode == 0 || KeyCode == 0xE0 ) // Escape character
        {
            uint32_t Func = _getch();
            if( Func == 0x48 ) // Up
            {
                if( !PrevCommands.empty() )
                {
                    if( PrevCommand != PrevCommands.begin() )
                    {
                        PrevCommand--;
                    }

                    CurCommand = *PrevCommand;
                    CurArg = CurCommand.size() - 1;
                }
            }
            else if( Func == 0x50 ) // Down
            {
                if( !PrevCommands.empty() )
                {
                    if( PrevCommand != PrevCommands.end() && PrevCommand != PrevCommands.end() - 1 )
                    {
                        PrevCommand++;
                        CurCommand = *PrevCommand;
                        CurArg = CurCommand.size() - 1;
                    }
                    else if( PrevCommand == PrevCommands.end() - 1 )
                    {
                        PrevCommand++;
                        CurArg = 0;
                        CurCommand.clear();
                    }
                    else
                    {
                        CurArg = 0;
                        CurCommand.clear();
                    }
                }
            }
            else if( Func == 0x4B ) // Left
            {
            }
            else if( Func == 0x4D ) // Right
            {
            }
            else
            {
                // Unknown function key
            }
        }
    }

    void Console::PrintLine()
    {
        SetTextColor(Color::Info);
        std::cout << '\r' << std::string(ConsoleWidth - 2, ' ') << '\r' << ps1beg;
        SetTextColor(Color::Input);

        for( size_t i = 0; i < CurCommand.size(); i++ )
        {
            if( Suggestion.length() && i == CurArg )
            {
                SetTextColor(Color::Suggestion);
                std::cout << Suggestion;
                std::cout << std::string(Suggestion.length(), '\b');
                SetTextColor(Color::Input);
                std::cout << CurCommand[i];
            }
            else
            {
                std::cout << CurCommand[i];
                if( i == CurCommand.size() - 1 )
                {
                    std::cout << ' ';
                }
            }
        }
        std::cout.flush();
    }

    void Console::PushCommand(const std::string& CommandName, std::shared_ptr<Command> Command)
    {
        Commands[CommandName] = Command;
    }
    void Console::PopCommand(const std::string& CommandName)
    {
        if( Commands.count(CommandName) )
        {
            Commands.erase(CommandName);
        }
    }

    bool Console::Run(const std::vector<std::string>& Args)
    {
        if( !Args.empty() )
        {
            if( !Args.front().compare("help") )
            {
                if( Args.size() >= 2 )
                {
                    if( Commands.count(Args[1]) )
                    {
                        if( Args.size() == 3 )
                        {
                            std::cout << Commands[Args[1]]->Info() << std::endl;
                        }
                        else
                        {
                            std::cout << Commands[Args[1]]->Info(Args.back()) << std::endl;
                        }
                    }
                    else
                    {
                        SetTextColor(Color::Error);
                        std::cout << "Command: " << Args[1] << "not found." << std::endl;
                    }
                }
                else
                {
                    // Show all command info
                    for( auto Command : Commands )
                    {
                        std::string Padded(Command.first);
                        Padded.resize(ConsoleWidth >> 1, '\xC4');
                        SetTextColor(Color::Info);
                        std::cout << Padded << std::endl;
                        SetTextColor(Color::Info^Color::Bright);
                        std::cout << Command.second->Info(Command.first) << std::endl;
                    }
                }
            }
            else if( !Args.front().compare("history") )
            {
                SetTextColor(Color::Info);
                for( auto Command : PrevCommands )
                {
                    for( auto Arg : Command )
                    {
                        std::cout << Arg << ' ';
                    }
                    std::cout << std::endl;
                }
            }
            else if( !Args.front().compare("quit") )
            {
                std::exit(0);
            }
        }
        return true;
    }

    std::string Console::Info(const std::string& Topic) const
    {
        if( !Topic.empty() )
        {
            if( !Topic.compare("help") )
            {
                return "Prints help for all commands\n"
                    "Type help (command name) (topic) to get help on a specific command";
            }
            else if( !Topic.compare("history") )
            {
                return "Displays all previously entered commands";
            }
            else if( !Topic.compare("quit") )
            {
                return "Quits the application";
            }
        }
        return "";
    };

    void SetTextColor(Color NewColor)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<std::underlying_type_t<Color>>(NewColor));
    }

    bool AllocateConsole(const std::string& ConsoleTitle)
    {
        // Allocate new console window
        if( !AllocConsole() )
        {
            MessageBox(nullptr, "Unable to allocate console", ConsoleTitle.c_str(), 0);
            return false;
        }

        CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;

        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleInfo);
        ConsoleInfo.dwSize.Y = 25;
        ConsoleInfo.dwSize.X = 30;
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), ConsoleInfo.dwSize);

        if( !SetConsoleTitle(ConsoleTitle.c_str()) )
        {
            MessageBox(nullptr, ("Unable set console title (Error code: " + std::to_string(GetLastError()) + ')').c_str(), ConsoleTitle.c_str(), 0);
            return false;
        }

        if( EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE | SC_MINIMIZE, MF_GRAYED) == -1 )
        {
            MessageBox(nullptr, "Unable to enable menu item", ConsoleTitle.c_str(), 0);
            return false;
        }

        if( !DrawMenuBar(GetConsoleWindow()) )
        {
            MessageBox(nullptr, ("Unable to DrawMenuBar (Error code: " + std::to_string(GetLastError()) + ')').c_str(), ConsoleTitle.c_str(), 0);
            return false;
        }

        // Reroute std streams
        //intptr_t hStd;
        //int32_t hConsole;
        //FILE *fp;

        //// Setup std output
        //hStd = reinterpret_cast<intptr_t>(GetStdHandle(STD_OUTPUT_HANDLE));
        //hConsole = _open_osfhandle(hStd, _O_TEXT);
        //fp = _fdopen(hConsole, "w");
        //*stdout = *fp;
        //setvbuf(stdout, nullptr, _IONBF, 0);

        //// Setup std input
        //hStd = reinterpret_cast<intptr_t>(GetStdHandle(STD_INPUT_HANDLE));
        //hConsole = _open_osfhandle(hStd, _O_TEXT);
        //fp = _fdopen(hConsole, "r");
        //*stdin = *fp;
        //setvbuf(stdin, nullptr, _IONBF, 0);

        //std::ios::sync_with_stdio();

        // New stream re-route method
        // MUCH faster
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        std::wcout.clear();
        std::cout.clear();
        std::wcerr.clear();
        std::cerr.clear();
        std::wcin.clear();
        std::cin.clear();

        return true;
    }

    size_t GetWidth()
    {
        CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
        if( GetConsoleScreenBufferInfo(
            GetStdHandle(STD_OUTPUT_HANDLE),
            &ConsoleInfo) )
        {
            return static_cast<size_t>(ConsoleInfo.dwSize.X);
        }
        return 0;
    }
}