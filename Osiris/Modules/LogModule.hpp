#pragma once
#include "OsirisModule.hpp"

class LogModule : public OsirisModule
{
public:
    LogModule();
    ~LogModule();

    void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime);
private:
};