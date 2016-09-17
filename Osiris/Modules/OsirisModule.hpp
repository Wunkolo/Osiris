#pragma once
#include <chrono>

#include "../Osiris.hpp"

class OsirisModule
{
public:
    virtual void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime) = 0;
    virtual ~OsirisModule() = default;
private:
};