#pragma once

#include <Utils/Singleton.hpp>
#include <Utils/Pointer.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <map>

class OsirisModule;

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
        //Console::Console::Instance()->PushCommand(Name, Module);
        // Add to Command list
        Commands[Name] = Module;
    }

private:
    std::map<std::string, std::shared_ptr<OsirisModule>> Commands;
};