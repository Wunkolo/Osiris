#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

namespace Console
{
    class Command
    {
    public:
        Command()
        {
        }
        ~Command()
        {
        }

        // Return true on success
        virtual bool Run(const std::vector<std::string> &Arguments) = 0;

        // Command and usage info
        virtual std::string Info(const std::string &Topic = "") const = 0;
    };

    enum class Color : uint8_t
    {
        None = 0,
        Bright = 0x8,
        Red = 0x4,
        Blue = 0x1,
        Green = 0x2,
        Yellow = Red | Green,
        Cyan = Green | Blue,
        Magenta = Red | Blue,
        Normal = Red | Blue | Green,
        // Bitshift colors left 4 bits to get background version
        BackBright = Bright << 4,
        BackRed = Red << 4,
        BackBlue = Blue << 4,
        BackGreen = Green << 4,
        BackYellow = Yellow << 4,
        BackCyan = Cyan << 4,
        BackMagenta = Magenta << 4,
        // Prefabs
        Error = Bright | Red,
        Info = Bright | Yellow,
        Input = Bright | Cyan,
        Suggestion = Cyan
    };

    typedef std::underlying_type<Color>::type ColorType;

    inline Color operator| (Color Left, Color Right)
    {
        return static_cast<Color>(
            static_cast<ColorType>(Left)
            | static_cast<ColorType>(Right)
            );
    }

    inline Color operator|= (Color Left, Color Right)
    {
        Left = static_cast<Color>(
            static_cast<ColorType>(Left)
            | static_cast<ColorType>(Right)
            );
        return Left;
    }

    inline Color operator^ (Color Left, Color Right)
    {
        return static_cast<Color>(
            static_cast<ColorType>(Left)
            ^ static_cast<ColorType>(Right)
            );
    }

    inline Color operator^= (Color Left, Color Right)
    {
        Left = static_cast<Color>(
            static_cast<ColorType>(Left)
            ^ static_cast<ColorType>(Right)
            );
        return Left;
    }

    class Console
    {
    public:
        ~Console();

        // Singleton
        Console(Console const&) = delete;
        Console& operator=(Console const&) = delete;
        inline static std::shared_ptr<Console> Instance()
        {
            static std::shared_ptr<Console> ConsoleInstance
                = std::shared_ptr<Console>(new Console());

            return ConsoleInstance;
        }

        void HandleInput(uint32_t KeyCode);
        void PrintLine();

        void PushCommand(const std::string &CommandName, std::shared_ptr<Command> Command);
        void PopCommand(const std::string &CommandName);
    protected:
        Console();
    private:
        // meta commands
        class Help : public Command
        {
        public:
            Help();
            ~Help();

            bool Run(const std::vector<std::string> &Arguments);
            std::string Info(const std::string &Topic = "") const;
        };

        class History : public Command
        {
        public:
            History();
            ~History();

            bool Run(const std::vector<std::string>& Arguments);
            std::string Info(const std::string& Topic) const;
        };

        class Quit : public Command
        {
        public:
            Quit();
            ~Quit();

            bool Run(const std::vector<std::string>& Arguments);
            std::string Info(const std::string& Topic) const;
        };

        friend class Help;
        friend class History;
        friend class Quit;

        std::map<std::string, std::shared_ptr<Command>> Commands;

        // History
        std::vector<std::string> PrevCommands;
        std::vector<std::string>::iterator PrevCommand;

        // Current Command
        std::string CurCommand;
        std::string CurSuggestion;

        std::string NearestCommand(const std::string &CurrentCommand) const;

        // Console Data
        size_t ConsoleWidth;
        void* ConsoleHandle;
    };

    bool AllocateConsole(const std::string& ConsoleTitle);

    void SetTextColor(Color Color);

    size_t GetWidth();
}