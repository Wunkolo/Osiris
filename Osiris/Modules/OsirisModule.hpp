#pragma once
#include <chrono>
#include <vector>
#include <string>

#include "../Osiris.hpp"

class OsirisModule
{
public:
    virtual void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime) = 0;
    virtual bool Execute(const std::vector<std::string> &Arguments)
    {
        return true;
    };
    virtual ~OsirisModule() = default;
private:
};