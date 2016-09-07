#include "ThothDebugger.hpp"

ThothDebugger::ThothDebugger(const std::wstring &PackageID)
    :
    PackageID(PackageID)
{
    /*DebugSettings.CoCreateInstance(
        CLSID_PackageDebugSettings,
        nullptr,
        CLSCTX_ALL
    );*/
    CoCreateInstance(
        CLSID_PackageDebugSettings,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&DebugSettings)
    );
}

ThothDebugger::~ThothDebugger()
{
    DebugSettings.Release();
}

void ThothDebugger::EnableDebugging(std::wstring &Arguments)
{
    DebugSettings->EnableDebugging(
        PackageID.c_str(),
        nullptr,
        nullptr
    );
}

void ThothDebugger::DisableDebugging()
{
    DebugSettings->DisableDebugging(
        PackageID.c_str()
    );
}

void ThothDebugger::Resume()
{
    DebugSettings->Resume(
        PackageID.c_str()
    );
}

void ThothDebugger::Suspend()
{
    DebugSettings->Suspend(
        PackageID.c_str()
    );
}

PACKAGE_EXECUTION_STATE ThothDebugger::GetExecutionState()
{
    PACKAGE_EXECUTION_STATE PackageState = PES_UNKNOWN;

    DebugSettings->GetPackageExecutionState(
        PackageID.c_str(),
        &PackageState
    );

    return PackageState;
}