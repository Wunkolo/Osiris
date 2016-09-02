#pragma once

#include <Utils/Singleton.hpp>
#include "Console/Console.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>

class ThothModule : public Console::Command
{
public:
    virtual void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime) = 0;
    virtual ~ThothModule() = default;
private:
};

class Thoth : public Util::Singleton<Thoth>
{
public:
    Thoth();
    ~Thoth();

    void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime);

    template<class T,
        class = typename std::enable_if<std::is_base_of<ThothModule, T>::value>::type>
        inline void PushModule(const std::string& Name)
    {
        std::shared_ptr<T> Module = std::make_shared<T>();

        // Push to command list
        Console::Console::Instance()->PushCommand(Name, Module);
        // Add to Command list
        Commands[Name] = Module;
    }

private:
    std::map<std::string, std::shared_ptr<ThothModule>> Commands;
};