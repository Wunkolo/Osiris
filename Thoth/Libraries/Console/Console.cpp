#include "Console.hpp"

#include <algorithm>
#include <sstream>

#include <Windows.h>
#include <conio.h> // _getch()
#pragma warning(disable:4996)

#include <io.h>
#include <cctype> //isgraph

#define ps1beg "\xAF["
#define ps1end "]\xAE"

// Utils
namespace
{
    std::vector<std::string> SplitString(const std::string &String, char Delimiter)
    {
        std::vector<std::string> SplitString;
        std::stringstream StringStream(String);
        std::string Token;
        while( std::getline(StringStream, Token, Delimiter) )
        {
            SplitString.push_back(Token);
        }
        return SplitString;
    }
}

namespace Console
{
    Console::Console()
    {
        ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleOutputCP(437);

        CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo{};
        if( GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleInfo) )
        {
            ConsoleWidth = ConsoleInfo.dwSize.X;
        }

        SetTextColor(Color::Info);

        PushCommand("help", std::make_shared<Help>());
        PushCommand("history", std::make_shared<History>());
        PushCommand("quit", std::make_shared<Quit>());
    }

    std::string Console::NearestCommand(const std::string & Command) const
    {
        if( !Command.empty() )
        {
            // Find closest matching command
            std::vector<std::string> Suggestions;

            for( const auto &Cmd : Commands )
            {
                if( !Command.compare(0, Command.length(), (Cmd.first), 0, Command.length()) )
                {
                    Suggestions.push_back((Cmd.first));
                }
            }

            if( Suggestions.size() )
            {
                // Return first match, for now
                return Suggestions.front();
            }
        }
        return "";
    }

    Console::~Console()
    {
        Commands.clear();
    }

    void Console::HandleInput(uint32_t KeyCode)
    {
        switch( KeyCode )
        {
        case '\r': // Enter
        {
            SetTextColor(Color::Info);
            // Clear previous suggestion
            if( !CurSuggestion.empty() )
            {
                std::cout.width(CurSuggestion.length());
                std::cout.fill(' ');
                std::cout << "";

                std::cout.width(CurSuggestion.length());
                std::cout.fill('\b');
                std::cout << "";
            }

            std::cout << ps1end << std::endl;

            std::string Command = CurCommand.substr(0, CurCommand.find_first_of(' '));

            if( !Command.empty() )
            {
                // Run command if it exists
                if( Commands.count(Command)
                    && Commands[Command] )
                {
                    if( !Commands[Command]->Run(SplitString(CurCommand, ' ')) )
                    {
                        // Error running command;
                        SetTextColor(Color::Error);
                        std::cout << "Invalid Usage: " << Command << std::endl;
                        SetTextColor(Color::Info);
                        std::cout << Commands[Command]->Info() << std::endl;
                    }
                }
                else
                {
                    SetTextColor(Color::Error);
                    std::cout << "Unknown Command: " << Command << std::endl;
                }

                // Add to history
                PrevCommands.push_back(CurCommand);
                PrevCommand = PrevCommands.end();
                CurCommand.clear();
                CurSuggestion.clear();
            }

            SetTextColor(Color::Info);
            break;
        }
        case '\t': // Tab
        {
            // Tab completion
            if( !CurSuggestion.empty() && !CurCommand.empty() )
            {
                CurCommand = CurSuggestion;
            }
            break;
        }
        case '\x16': // Ctrl + V
        {
            // Paste in clipbard
            if( OpenClipboard(nullptr) )
            {
                std::string Clipboard(reinterpret_cast<char*>(GetClipboardData(CF_TEXT)));
                CloseClipboard();
                for( const char &CurChar : Clipboard )
                {
                    HandleInput(CurChar);
                }
            }
            break;
        }
        case 0x00: // Function Keys
        case 0xE0:
        {
            uint32_t Func = _getch();
            switch( Func )
            {
            case 0x48: // Up
            {
                if( !PrevCommands.empty() )
                {
                    if( PrevCommand != PrevCommands.begin() )
                    {
                        PrevCommand--;
                    }

                    CurCommand = *PrevCommand;
                }
                break;
            }
            case 0x50: // Down
            {
                if( !PrevCommands.empty() )
                {
                    if( PrevCommand != PrevCommands.end() && PrevCommand != PrevCommands.end() - 1 )
                    {
                        PrevCommand++;
                        CurCommand = *PrevCommand;
                    }
                    else if( PrevCommand == PrevCommands.end() - 1 )
                    {
                        PrevCommand++;
                        CurCommand.clear();
                    }
                    else
                    {
                        CurCommand.clear();
                    }
                }
                break;
            }
            }
            break;
        }
        case '\b': // Backspace
        {
            if( !CurCommand.empty() )
            {
                // Remove character from current command
                CurCommand.pop_back();
                // Update Suggestion
                CurSuggestion = NearestCommand(CurCommand);
            }
            break;
        }
        default: // Every other character (alphanum and space)
        {
            if( std::isgraph(KeyCode) || std::isspace(KeyCode) )
            {
                CurCommand.push_back(static_cast<uint8_t>(KeyCode));

                // Update Suggestion
                CurSuggestion = NearestCommand(CurCommand);
            }
            break;
        }
        }
    }

    void Console::PrintLine()
    {
        SetTextColor(Color::Info);
        std::cout << '\r';
        std::cout.width(ConsoleWidth - 1);
        std::cout.fill(' ');
        std::cout << '\r' << ps1beg;
        SetTextColor(Color::Input);

        if( CurSuggestion.length() )
        {
            // Print suggestion
            SetTextColor(Color::Suggestion);
            std::cout << CurSuggestion;
            // Go back
            std::cout.width(CurSuggestion.length());
            std::cout.fill('\b');
            std::cout << "";
        }
        // Print currently typed in command
        SetTextColor(Color::Input);
        std::cout << CurCommand;
        std::cout.flush();
    }

    void Console::PushCommand(const std::string &CommandName, std::shared_ptr<Command> Command)
    {
        Commands[CommandName] = Command;
    }
    void Console::PopCommand(const std::string &CommandName)
    {
        if( Commands.count(CommandName) )
        {
            Commands.erase(CommandName);
        }
    }

    /// Meta commands

    // Help
    Console::Help::Help()
    {
    }

    Console::Help::~Help()
    {
    }

    bool Console::Help::Run(const std::vector<std::string> &Arguments)
    {
        if( Arguments.size() >= 2 )
        {
            if( Console::Instance()->Commands.count(Arguments[1]) )
            {
                if( Arguments.size() == 3 )
                {
                    std::cout << Console::Instance()->Commands[Arguments[1]]->Info() << std::endl;
                }
                else
                {
                    std::cout << Console::Instance()->Commands[Arguments[1]]->Info(Arguments.back()) << std::endl;
                }
            }
            else
            {
                SetTextColor(Color::Error);
                std::cout << "Command: " << Arguments[1] << " not found." << std::endl;
            }
        }
        else
        {
            // Show all command info
            for( const auto &Command : Console::Instance()->Commands )
            {
                std::string Padded(Command.first);
                Padded.resize(Console::Instance()->ConsoleWidth / 2, '\xC4');
                SetTextColor(Color::Info);
                std::cout << Padded << std::endl;
                SetTextColor(Color::Info^Color::Bright);
                std::cout << Command.second->Info(Command.first) << std::endl;
            }
        }
        return true;
    }

    std::string Console::Help::Info(const std::string & Topic) const
    {
        return "Prints help for all commands\n"
            "Type help (command name) (topic) to get help on a specific command";
    }

    // History
    Console::History::History()
    {
    }

    Console::History::~History()
    {
    }

    bool Console::History::Run(const std::vector<std::string>& Arguments)
    {
        SetTextColor(Color::Info);
        for( const auto &Command : Console::Instance()->PrevCommands )
        {
            std::cout << Command << std::endl;
        }
        return true;
    }

    std::string Console::History::Info(const std::string & Topic) const
    {
        return "Displays all previously entered commands";
    }

    // Quit
    Console::Quit::Quit()
    {
    }

    Console::Quit::~Quit()
    {
    }

    bool Console::Quit::Run(const std::vector<std::string>& Arguments)
    {
        std::exit(0);
        return true;
    }

    std::string Console::Quit::Info(const std::string & Topic) const
    {
        return "Quits the application";
    }

    // Static Functions

    void SetTextColor(Color NewColor)
    {
        SetConsoleTextAttribute(
            GetStdHandle(STD_OUTPUT_HANDLE),
            static_cast<std::underlying_type_t<Color>>(NewColor)
        );
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

        GetConsoleScreenBufferInfo(
            GetStdHandle(STD_OUTPUT_HANDLE),
            &ConsoleInfo
        );

        ConsoleInfo.dwSize.Y = 25;
        ConsoleInfo.dwSize.X = 30;

        SetConsoleScreenBufferSize(
            GetStdHandle(STD_OUTPUT_HANDLE),
            ConsoleInfo.dwSize
        );

        if( !SetConsoleTitle(ConsoleTitle.c_str()) )
        {
            MessageBox(
                nullptr,
                ("Unable set console title (Error code: " + std::to_string(GetLastError()) + ')').c_str(),
                ConsoleTitle.c_str(),
                0
            );
            return false;
        }

        if( EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE | SC_MINIMIZE, MF_GRAYED) == -1 )
        {
            MessageBox(
                nullptr,
                "Unable to enable menu item",
                ConsoleTitle.c_str(),
                0);
            return false;
        }

        if( !DrawMenuBar(GetConsoleWindow()) )
        {
            MessageBox(
                nullptr,
                ("Unable to DrawMenuBar (Error code: " + std::to_string(GetLastError()) + ')').c_str(),
                ConsoleTitle.c_str(),
                0
            );
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