#pragma once

#include <string>

#include <ShObjIdl.h>
#include <atlbase.h>

// Class used to debug UWP Applications

class OsirisDebugger
{
public:
    OsirisDebugger(const std::wstring &PackageID);
    ~OsirisDebugger();

    void EnableDebugging(std::wstring &Arguments);
    void DisableDebugging();

    void Resume();
    void Suspend();

    PACKAGE_EXECUTION_STATE GetExecutionState();

private:
    ATL::CComQIPtr<IPackageDebugSettings> DebugSettings;
    std::wstring PackageID;
};