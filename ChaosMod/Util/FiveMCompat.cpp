#include <stdafx.h>

#include "FiveMCompat.h"

#include <array>

namespace
{
	constexpr FiveMCompat::NativeOffsetProfile kDefaultOffsets = { .DrawRectSetCoordsOffset = -1, .NetworkStateOffset = -1 };
	constexpr FiveMCompat::NativeOffsetProfile kOffsets3717    = { .DrawRectSetCoordsOffset = 0x9A, .NetworkStateOffset = 0x1C8 };
	constexpr FiveMCompat::NativeOffsetProfile kOffsets3757    = { .DrawRectSetCoordsOffset = 0x9F, .NetworkStateOffset = 0x1D0 };

	bool HasAnyFiveMModule()
	{
		static const std::array<const wchar_t *, 10> kKnownModules = { {
			L"gta-net-five.dll",
			L"gta-core-five.dll",
			L"net-five.dll",
			L"adhesive.dll",
			L"citizen-resources-client.dll",
			L"ros-patches-five.dll",
			L"citizen-scripting-gta.dll",
			L"citizen-scripting-core.dll",
			L"citizen-scripting-lua.dll",
			L"asi-five.dll",
		} };

		return std::any_of(kKnownModules.begin(), kKnownModules.end(), [](auto module) { return GetModuleHandle(module) != nullptr; });
	}
}

namespace FiveMCompat
{
	bool IsFiveMClient()
	{
		static const bool isFiveM = HasAnyFiveMModule();
		return isFiveM;
	}

	ClientBuild GetClientBuild()
	{
		static const ClientBuild build = []()
		{
			if (!IsFiveMClient())
				return ClientBuild::None;

			const int runtimeBuild = GetGameBuildNumber();
			switch (runtimeBuild)
			{
			case 3717:
				return ClientBuild::Build3717;
			case 3757:
				return ClientBuild::Build3757;
			default:
				return ClientBuild::Unsupported;
			}
		}();

		return build;
	}

	const NativeOffsetProfile &GetNativeOffsetProfile()
	{
		switch (GetClientBuild())
		{
		case ClientBuild::Build3717:
			return kOffsets3717;
		case ClientBuild::Build3757:
			return kOffsets3757;
		default:
			return kDefaultOffsets;
		}
	}

	const char *GetClientBuildString()
	{
		switch (GetClientBuild())
		{
		case ClientBuild::None:
			return "Standalone GTA V";
		case ClientBuild::Build3717:
			return "FiveM 3717";
		case ClientBuild::Build3757:
			return "FiveM 3757";
		default:
			return "FiveM (unsupported build)";
		}
	}

	bool IsSupportedClient()
	{
		const auto build = GetClientBuild();
		return build == ClientBuild::None || build == ClientBuild::Build3717 || build == ClientBuild::Build3757;
	}

	bool IsHookSupported(const char *hookName)
	{
#if defined(CHAOS_ENABLE_FIVEM_37XX)
		if (!IsFiveMClient())
			return true;

		// FiveM replaces portions of RAGE/SC networking internals; these hooks are unstable there.
		if (!strcmp(hookName, "ApplyChangeSetEntry") || !strcmp(hookName, "ModelSpawnBypass"))
			return false;
#endif
		return true;
	}

	void LogCompatibilityReport()
	{
		LOG("Compatibility: host=" << GetClientBuildString() << ", gtaMode=" << (IsEnhanced() ? "Enhanced" : "Legacy")
		                         << ", runtimeBuild=" << GetGameBuildNumber());

		if (IsFiveMClient())
		{
			if (IsSupportedClient())
				LOG("Compatible client FiveM detected: " << GetClientBuildString() << " (low-level hooks gated)");
			else
				LOG("Warning: FiveM client detected but build is unsupported. ChaosMod will continue with conservative fallbacks.");

			LOG("Loader note: ScriptHookV/ASI side-loading is limited on modern FiveM. Development hot-reload is disabled there.");
		}
	}
}
