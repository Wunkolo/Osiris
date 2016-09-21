#include "PackageDump.hpp"

#include <Windows.h>
#include <Shlobj.h>

// Package Query API
#include <appmodel.h>

#include <Utils/Utils.hpp>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

PackageDump::PackageDump()
{
}

PackageDump::~PackageDump()
{
}

void PackageDump::Tick(const std::chrono::high_resolution_clock::duration &DeltaTime)
{
}

bool PackageDump::Execute(const std::vector<std::string> &Arguments)
{
    wchar_t UserPath[MAX_PATH] = { 0 };
    SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, 0, UserPath);

    uint32_t FamilyNameLength = 0;
    std::wstring DumpPath;
    GetCurrentPackageFamilyName(&FamilyNameLength, nullptr);
    DumpPath.resize(FamilyNameLength - 1);
    GetCurrentPackageFamilyName(&FamilyNameLength, &DumpPath[0]);

    DumpPath = fs::path(UserPath) / L"AppData/Local/Packages" / DumpPath / L"/TempState/DUMP";

    LOG << "Dumping files..." << std::endl;
    LOG << "Dump path: " << DumpPath << std::endl;

    try
    {
        fs::copy(
            L".",
            DumpPath,
            fs::copy_options::recursive | fs::copy_options::update_existing);
    }
    catch( fs::filesystem_error &e )
    {
        LOG << "Error dumping: " << e.what() << std::endl;
        LOG << "\tOperand 1: " << e.path1() << std::endl;
        LOG << "\tOperand 2: " << e.path2() << std::endl;
        return false;
    }
    LOG << "Dumping complete!" << std::endl;
    return true;
}