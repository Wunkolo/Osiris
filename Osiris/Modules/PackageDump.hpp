#pragma once
#include "OsirisModule.hpp"

class PackageDump : public OsirisModule
{
public:
    PackageDump();
    ~PackageDump();

    void Tick(const std::chrono::high_resolution_clock::duration &DeltaTime);

    bool Execute(const std::vector<std::string> &Arguments);
private:
};