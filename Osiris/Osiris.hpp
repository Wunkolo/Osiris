#pragma once

#include <Utils/Singleton.hpp>
#include "Console/Console.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>

class OsirisModule : public Console::Command
{
public:
    virtual void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime) = 0;
    virtual ~OsirisModule() = default;
private:
};

class Osiris : public Util::Singleton<Osiris>
{
public:
    Osiris();
    ~Osiris();

    void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime);

    template<class T,
        class = typename std::enable_if<std::is_base_of<OsirisModule, T>::value>::type>
        inline void PushModule(const std::string& Name)
    {
        std::shared_ptr<T> Module = std::make_shared<T>();

        // Push to command list
        Console::Console::Instance()->PushCommand(Name, Module);
        // Add to Command list
        Commands[Name] = Module;
    }

private:
    std::map<std::string, std::shared_ptr<OsirisModule>> Commands;
};