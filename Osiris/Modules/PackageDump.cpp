#include "PackageDump.hpp"

#include <UWP/UWP.hpp>
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
    std::wstring DumpPath = fs::path(UWP::Current::Storage::GetTempStatePath()) / L"DUMP";

    LOG << "Dumping files..." << std::endl;
    LOG << "Dump path: " << DumpPath << std::endl;

    try
    {
        fs::copy(
            UWP::Current::GetPackagePath(),
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