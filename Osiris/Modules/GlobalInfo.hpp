#pragma once
#include "OsirisModule.hpp"

class GlobalInfo : public OsirisModule
{
public:
    GlobalInfo();
    ~GlobalInfo();

    void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime);
private:
};