#include "PackageDump.hpp"

#include <Windows.h>
#include <Shlobj.h>

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

    LOG << "Dumping files..." << std::endl;
    LOG << "Dump path: " << (fs::path(UserPath) / L"AppData/Local/Packages/Microsoft.Halo5Forge_8wekyb3d8bbwe/TempState/DUMP") << std::endl;

    try
    {
        fs::copy(
            L".",
            fs::path(UserPath) / L"AppData/Local/Packages/Microsoft.Halo5Forge_8wekyb3d8bbwe/TempState/DUMP",
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