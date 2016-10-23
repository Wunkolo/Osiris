#include <iostream>
#include <iomanip>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Osiris.hpp"

#include "Modules/OsirisModule.hpp"
#include "Modules/OsirisModules.hpp"

#include <UWP/UWP.hpp>
#include <Utils/Utils.hpp>
#include <Ausar/Ausar.hpp>

#include <vector>
#include <iterator>

#include <regex>
#include <thread>
#include <set>
#include <mutex>
#include "TagName.hpp"

uint32_t Run = 0;

std::vector<Util::Pointer> ModelArray;
std::mutex ModelMutex;

std::vector<uint32_t> ModelIDArray;
std::mutex ModelIDMutex;

Osiris::Osiris()
{
    std::wstring WorkingPath = UWP::Current::Storage::GetTempStatePath();

    Util::Log::Instance()->SetFile(WorkingPath + L"\\Log.txt");

    LOG << "Osiris" << "---- ";
    LOG << '[' << __DATE__ << " : " << __TIME__ << ']' << std::endl;
    LOG << "\t-https://github.com/Wunkolo/Osiris\n";
    LOG << "Working Path: " << WorkingPath << std::endl;
    LOG << std::wstring(80, '-') << std::endl;

    LOG << "Publisher: " << UWP::Current::GetPublisher() << std::endl;
    LOG << "Publisher ID: " << UWP::Current::GetPublisherID() << std::endl;
    LOG << "Publisher Path: " << UWP::Current::Storage::GetPublisherPath() << std::endl;

    LOG << "Package Path: " << UWP::Current::GetPackagePath() << std::endl;
    LOG << "Package Name: " << UWP::Current::GetFullName() << std::endl;
    LOG << "Family Name: " << UWP::Current::GetFamilyName() << std::endl;

    LOG << std::hex << std::uppercase << std::setfill(L'0')
        << "Process Base: 0x" << Util::Process::Base() << std::endl;
    LOG << "Osiris Thread ID: 0x" << Util::Thread::GetCurrentThreadId() << std::endl;
    LOG << "Osiris Base: 0x" << Util::Process::GetModuleBase("Osiris.dll") << std::endl;

    //LOG << "---- Loaded Modules ----" << std::endl;
    //Util::Process::IterateModules(
    //    [](const char* Name, const char* Path, Util::Pointer Base, size_t Size) -> bool
    //{
    //    LOG << "0x" << std::hex << Base << " - 0x" << std::hex << Base(Size) << " | " << Path << std::endl;
    //    return true;
    //}
    //);

    //InitTagNames();

    //LOG << "Finding pattern" << std::endl;

    //Run = 1;
    //std::thread CycleThread(
    //    []()
    //{
    //    while( true )
    //    {
    //        ModelMutex.lock();
    //        ModelIDMutex.lock();
    //        for( const auto& ModelPtr : ModelArray )
    //        {
    //            // Assign each Model pointer another random model
    //            ModelPtr.Write<uint32_t>(ModelIDArray[rand() % ModelIDArray.size()]);
    //        }
    //        ModelIDMutex.unlock();
    //        ModelMutex.unlock();
    //        std::this_thread::sleep_for(std::chrono::milliseconds(125));
    //    }
    //}
    //);
    //CycleThread.detach();

    //Util::Process::IterateReadableMemory(
    //    [](Util::Pointer Base, size_t Size) -> bool
    //{
    //    [&]()
    //    {//LOG << "Region: " << Base << " | " << Size << std::endl;
    //    //7F0000???????? global_id ????????00000000tagclassruntime_id
    //        static const std::regex const Expression(
    //            R"(\x7F\x00\x00[\x00-\xFF]{12}[\x00]{4}jorp)",
    //            std::regex::optimize | std::regex::nosubs
    //        );

    //        std::cregex_iterator Start = std::cregex_iterator(
    //            Base.Point<const char>(),
    //            Base(Size).Point<const char>(),
    //            Expression
    //        );

    //        std::cregex_iterator End = std::cregex_iterator();

    //        for( std::cregex_iterator i = Start; i != End; ++i )
    //        {
    //            const std::cmatch Match = *i;

    //            uint32_t GlobalID;
    //            uint32_t RuntimeID;

    //            [&]()
    //            {
    //                __try
    //                {
    //                    GlobalID = Base(Match.position())(7).Read<uint32_t>();
    //                    RuntimeID = Base(Match.position())(23).Read<uint32_t>();
    //                }
    //                __except( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ?
    //                    EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH )
    //                {
    //                    [&]()
    //                    {
    //                        LOG << "Yikes at" << Base << "!!!" << std::endl;
    //                    }();

    //                    GlobalID = 0;
    //                    RuntimeID = 0;
    //                }
    //            }();

    //            const char * Name = GetTagNameFromGlobalID(GlobalID);
    //            if( Name == nullptr )
    //            {
    //                Name = "Not found";
    //            }
    //            LOG << "Offset: " << Base(Match.position()) << std::endl;
    //            LOG << "\tName: " << Name << std::endl;
    //            LOG << "\tGlobalID: " << GlobalID << std::endl;
    //            LOG << "\tRuntimeID: " << RuntimeID << std::endl;

    //            if(
    //                (strstr(Name, R"(objects)") != nullptr)
    //                //(strstr(Name, R"(.sound)") != nullptr)
    //                //&& (strstr(Name, R"(sound\001_vo)") == nullptr)//if its vo, skip it
    //                //(strstr(Name, R"(objects\equipment)") != nullptr)
    //                //|| (strstr(Name, R"(levels\assets)") != nullptr)
    //                //|| (strstr(Name, R"(levels\multi)") != nullptr)
    //                //|| (strstr(Name, R"(levels\forge)") != nullptr)
    //                //|| (strstr(Name, R"(objects\vehicles)") != nullptr)
    //                //|| (strstr(Name, R"(objects\weapons)") != nullptr)
    //                //|| (strstr(Name, R"(objects\characters)") != nullptr)
    //                //|| (strstr(Name, R"(objects\levels)") != nullptr)
    //                //|| (strstr(Name, R"(objects\temp)") != nullptr)
    //                )
    //            {
    //                LOG << "==========================Added to list" << std::endl;
    //                ModelIDMutex.lock();
    //                ModelIDArray.push_back(RuntimeID);
    //                // Remove duplicates
    //                std::set<uint32_t> Unique(ModelIDArray.begin(), ModelIDArray.end());
    //                ModelIDArray.assign(Unique.begin(), Unique.end());
    //                ModelIDMutex.unlock();

    //                ModelMutex.lock();
    //                ModelArray.push_back(Base(Match.position())(23));
    //                ModelMutex.unlock();
    //            }
    //        }
    //    }();

    //    return true;
    //}
    //);

    //LOG << "Done" << std::endl;

    //Spartan = 0;

    // Push Commands
    //PushModule<LogModule>("logging");
    //PushModule<GlobalInfo>("globals");
    PushModule<PackageDump>("dump");

    TODO("Config file to enable dumping of files");
    // Uncomment this to enable dumping
    RunModule("dump");
}

Osiris::~Osiris()
{
}

void Osiris::Tick(const std::chrono::high_resolution_clock::duration &DeltaTime)
{
    for( const std::pair<std::string, std::shared_ptr<OsirisModule>>& Command : Commands )
    {
        if( Command.second )
        {
            Command.second->Tick(DeltaTime);
        }
    }
}

bool Osiris::RunModule(const std::string &Name, const std::vector<std::string> &Arguments)
{
    std::map<std::string, std::shared_ptr<OsirisModule>>::const_iterator CommandIter;
    CommandIter = Commands.find(Name);
    if( CommandIter != Commands.end() )
    {
        return CommandIter->second.get()->Execute(Arguments);
    }
    return false;
}