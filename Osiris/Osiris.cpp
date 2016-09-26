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
#include <mutex>
#include "TagName.hpp"

uint32_t DebugMaterial = 0;

std::vector<Util::Pointer> Whiten;
std::mutex WhitenMutex;

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

    InitTagNames();

    LOG << "Finding pattern" << std::endl;
    Util::Process::IterateReadableMemory(
        [](Util::Pointer Base, size_t Size) -> bool
    {
        __try
        {
            [&]() {//LOG << "Region: " << Base << " | " << Size << std::endl;
            //7F0000???????? global_id ????????00000000tagclassruntime_id
                static const std::regex const Expression(
                    R"(\x7F\x00\x00[\x00-\xFF]{12}[\x00]{4}\x20tam)",
                    std::regex::optimize | std::regex::nosubs
                );

                std::cregex_iterator Start = std::cregex_iterator(
                    Base.Point<const char>(),
                    Base(Size).Point<const char>(),
                    Expression
                );

                std::cregex_iterator End = std::cregex_iterator();

                for( std::cregex_iterator i = Start; i != End; i++ )
                {
                    const std::cmatch Match = *i;
                    uint32_t GlobalID = Base(Match.position())(7).Read<uint32_t>();
                    uint32_t RuntimeID = Base(Match.position())(23).Read<uint32_t>();
                    const char * Name = GetTagNameFromGlobalID(GlobalID);
                    if( Name == nullptr )
                    {
                        Name = "Not found";
                    }
                    LOG << "Offset: " << Base(Match.position()) << std::endl;
                    LOG << "\tName: " << Name << std::endl;
                    LOG << "\tGlobalID: " << GlobalID << std::endl;
                    LOG << "\tRuntimeID: " << RuntimeID << std::endl;

                    if( GlobalID == 0x694D )
                    {
                        LOG << "==========================Found Debug Material (thread created)" << std::endl;
                        DebugMaterial = RuntimeID;
                        std::thread WhitenThread(
                            []()
                        {
                            while( DebugMaterial != 0 )
                            {
                                WhitenMutex.lock();
                                if( Whiten.size() != 0 )
                                {
                                    for( const Util::Pointer &Id : Whiten )
                                    {
                                        Id.Write<uint32_t>(DebugMaterial);
                                    }
                                    Whiten.clear();
                                }
                                WhitenMutex.unlock();
                            }
                        }
                        );
                        WhitenThread.detach();
                    }
                    if( (strstr(Name, R"(levels\)") != nullptr)
                        || (strstr(Name, R"(objects\)") != nullptr)
                        )
                    {
                        LOG << "==========================Tagged for write" << std::endl;
                        WhitenMutex.lock();
                        Whiten.push_back(Base(Match.position())(23));
                        WhitenMutex.unlock();
                    }
                }
            }();
        }
        __except( [&](LPEXCEPTION_POINTERS p) -> int
        {
            LOG << "yikes at " << Base << "!!!" << std::endl;
            return EXCEPTION_CONTINUE_SEARCH;
        }(GetExceptionInformation()) )
        {
            return false;
        }
        return true;
    }
    );

    LOG << "Done" << std::endl;

    DebugMaterial = 0;

    // Push Commands
    PushModule<LogModule>("logging");
    PushModule<GlobalInfo>("globals");
    PushModule<PackageDump>("dump");

    TODO("Config file to enable dumping of files");
    // Uncomment this to enable dumping
    //RunModule("dump");
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