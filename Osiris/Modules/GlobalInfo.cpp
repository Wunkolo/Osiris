#include "GlobalInfo.hpp"

#include <Ausar/Ausar.hpp>
#include <Utils/Utils.hpp>

GlobalInfo::GlobalInfo()
{
    LOG << "Dumping globals in 3 seconds..." << std::endl;
}

GlobalInfo::~GlobalInfo()
{
}

void GlobalInfo::Tick(const std::chrono::high_resolution_clock::duration & DeltaTime)
{
    static std::chrono::high_resolution_clock::duration Timer
        = std::chrono::high_resolution_clock::duration::zero();
    static bool Dumped = false;

    Timer += DeltaTime;

    // Wait a bit for globals to be allocated, find a better, determinant way
    // 3 seconds seems to be fast enough
    if( !Dumped && std::chrono::duration_cast<std::chrono::seconds>(Timer).count() > 3 )
    {
        const Ausar::Threading::ThreadTable *Table;

        Table = Util::Process::GetModuleBase()(0x58CA4B0).Point<Ausar::Threading::ThreadTable>();

        uint32_t ThreadID = Table->GetThreadIDByName("MAIN");

        LOG << "Main Thread ID: 0x" << ThreadID << std::endl;

        Util::Pointer Tls;

        if( Util::Thread::GetThreadLocalStorage(ThreadID, 0, Tls) )
        {
            LOG << "Tls Globals: " << Tls << std::endl;

            LOG << "Physics Constants: " << Tls[0x2D30] << std::endl;
            LOG << "userGraphicsScalingOptions: " << Tls[0x3050] << std::endl;

            LOG << "random math: " << Tls[0x2C28] << std::endl;
            LOG << "incident globals: " << Tls[0x2C38] << std::endl;

            LOG << "DOF Globals: " << Tls[0x49B0] << std::endl;
            LOG << "DOF Data: " << Tls[0x1310] << std::endl;
            LOG << "Director globals: " << Tls[0x198] << std::endl;
            LOG << "Hue saturation control: " << Tls[0x2FF8] << std::endl;
            LOG << "Game engine globals: " << Tls[0x13A8] << std::endl;
            LOG << "Local Game engine globals: " << Tls[0x13B0] << std::endl;
            LOG << "Game engine render globals: " << Tls[0x13B8] << std::endl;
            LOG << "Game time globals: " << Tls[0x12A8] << std::endl;
            LOG << "game_state_header: " << Tls[0x1280] << std::endl;
            LOG << "Composer globals: " << Tls[0x1C8] << std::endl;
            LOG << "Fp weapons: " << Tls[0x1260] << std::endl;

            LOG << "Player Focus: " << Tls[0x1320] << std::endl;
            LOG << "Player Control Globals: " << Tls[0x1340] << std::endl;
            LOG << "Player Control Globals Deter.: " << Tls[0x1348] << std::endl;
            LOG << "Player Globals: " << Tls[0x1370] << std::endl;
            LOG << "Player Mapping Globals: " << Tls[0x1350] << std::endl;

            LOG << "Player Data: " << Tls[0x1368] << std::endl;

            LOG << "AI Globals: " << Tls[0x2E40] << std::endl;
            LOG << "AI Player state Globals: " << Tls[0x2E18] << std::endl;

            LOG << "Interaction ripples: " << Tls[0x4960] << std::endl;

            LOG << "Rasterizer: " << Tls[0x49A0] << std::endl;
            LOG << "Render game globals: " << Tls[0x49A8] << std::endl;
            LOG << "Render texture globals: " << Tls[0x3058] << std::endl;
            LOG << "atmosphere override settings: " << Tls[0x4998] << std::endl;
            LOG << "fp orientations: " << Tls[0x4A10] << std::endl;

            LOG << "Objects: " << Tls[0x4B18] << std::endl;
            LOG << "Object name list: " << Tls[0x4B20] << std::endl;
            LOG << "Object placement globals: " << Tls[0x4B58] << std::endl;
            LOG << "Object globals: " << Tls[0x4C08] << std::endl;
            LOG << "orientations: " << Tls[0x110] << std::endl;
        }
        Dumped = true;
    }
}