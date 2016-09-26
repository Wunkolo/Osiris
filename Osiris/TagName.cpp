#include "TagName.hpp"
#include <unordered_map>
#include <Utils/Utils.hpp>
#include <UWP/UWP.hpp>

#include <fstream>
#include <iomanip>

std::unordered_map<uint32_t, std::string> Tags;

void InitTagNames()
{
    std::fstream fIn(
        UWP::Current::Storage::GetTempStatePath() + L"\\TagNames.txt"
    );

    LOG << "Loading Tagnames.txt" << std::endl;

    uint32_t TagID;
    std::string TagName;
    if( fIn.is_open() )
    {
        while( fIn >> std::hex >> TagID >> TagName )
        {
            Tags[TagID] = TagName;
        }
    }
    fIn.close();
    LOG << "Loaded Tagnames.txt" << std::endl;
}

const char * GetTagNameFromGlobalID(uint32_t GlobalID)
{
    if( Tags.size() == 0 )
    {
        InitTagNames();
    }

    if( Tags.count(GlobalID) )
    {
        return Tags[GlobalID].c_str();
    }
    return nullptr;
}