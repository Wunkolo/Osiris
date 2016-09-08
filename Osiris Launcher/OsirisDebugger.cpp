#include "OsirisDebugger.hpp"

OsirisDebugger::OsirisDebugger(const std::wstring &PackageID)
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

OsirisDebugger::~OsirisDebugger()
{
    DebugSettings.Release();
}

void OsirisDebugger::EnableDebugging(std::wstring &Arguments)
{
    DebugSettings->EnableDebugging(
        PackageID.c_str(),
        nullptr,
        nullptr
    );
}

void OsirisDebugger::DisableDebugging()
{
    DebugSettings->DisableDebugging(
        PackageID.c_str()
    );
}

void OsirisDebugger::Resume()
{
    DebugSettings->Resume(
        PackageID.c_str()
    );
}

void OsirisDebugger::Suspend()
{
    DebugSettings->Suspend(
        PackageID.c_str()
    );
}

PACKAGE_EXECUTION_STATE OsirisDebugger::GetExecutionState()
{
    PACKAGE_EXECUTION_STATE PackageState = PES_UNKNOWN;

    DebugSettings->GetPackageExecutionState(
        PackageID.c_str(),
        &PackageState
    );

    return PackageState;
}