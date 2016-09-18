#pragma once
#include <stdint.h>

namespace Ausar
{
	namespace Game
	{
		// Camera modes with vtable offsets
		enum class CameraMode : uint32_t
		{
			None = 0x332E348,
			Following = 0x332E788,
			Orbiting = 0x332F1B8,
			Flying = 0x332E6C0,
			FirstPerson = 0x332E520,
			Dead = 0x332DE48,
			Static = 0x332F678,
			Scripted = 0x332D6F8,
			Authored = 0x332C700,
			Respawning = 0x332F3A8,
			AnimatedTrick = 0x332C5F8,
			TrackedTarget = 0x332F708,
			//SpaceFighter = 0x332F5E0, // this crashes the game
			RemoteStrike = 0x332F318,
			KillCamProjectile = 0x332F278,
			VehicleBay = 0x332F798,
			// ActiveUser?
			// User0?
			// User1?
			// AllUsers?
		};
	}
}