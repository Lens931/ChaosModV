#pragma once

#include "game.h"
#include "Util/Natives.h"
#include "Util/Script.h"

namespace FiveMCompat
{
	enum class ClientBuild
	{
		None,
		Unsupported,
		Build3717,
		Build3757
	};

	struct NativeOffsetProfile
	{
		int DrawRectSetCoordsOffset = -1;
		int NetworkStateOffset      = -1;
	};

	bool IsFiveMClient();
	ClientBuild GetClientBuild();
	const NativeOffsetProfile &GetNativeOffsetProfile();
	const char *GetClientBuildString();
	bool IsSupportedClient();

	// Keep low-level hooks optional on modern FiveM builds where private symbols change frequently.
	bool IsHookSupported(const char *hookName);

	// Centralized wrappers for natives that changed behavior/signatures between SHV and modern FiveM.
	inline void Wait(DWORD timeMs)
	{
		WAIT(timeMs);
	}

	inline bool IsScreenFadedOut()
	{
		return IS_SCREEN_FADED_OUT();
	}

	inline void DoScreenFadeIn(int durationMs)
	{
		DO_SCREEN_FADE_IN(durationMs);
	}

	inline void SetEntityHealth(Entity entity, int health)
	{
		// FiveM 37xx keeps the 3-argument variant. Keep damage cause explicit to avoid accidental ABI drift.
		SET_ENTITY_HEALTH(entity, health, 0);
	}

	inline int GetGameBuildNumber()
	{
		return GET_GAME_BUILD();
	}

	void LogCompatibilityReport();
}
