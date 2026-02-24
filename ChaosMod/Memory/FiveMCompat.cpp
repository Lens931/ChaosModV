#include <stdafx.h>

#include "FiveMCompat.h"

#include "Memory.h"
#include "Util/Logging.h"

#include <algorithm>
#include <array>
#include <charconv>

namespace
{
	using namespace Memory;
	using namespace Memory::FiveMCompat;

	constexpr std::array<const wchar_t *, 10> g_FiveMModules = { {
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

	
	// Static compatibility metadata used by low-level wrappers. These are integration-layer offsets
	// (delta/index values), not absolute addresses, so they remain safe across ASLR.
	constexpr NativeOffsets g_Build3717Offsets { 0x14, 0x20, 3 };
	constexpr NativeOffsets g_Build3757Offsets { 0x18, 0x24, 3 };
	constexpr NativeOffsets g_DefaultOffsets { 0x0, 0x0, 3 };

	int ParseBuildNumber(const std::string &build)
	{
		if (build.empty())
			return 0;

		int parsedBuild = 0;
		auto [ptr, ec]  = std::from_chars(build.data(), build.data() + build.size(), parsedBuild);
		if (ec != std::errc() || ptr == build.data())
			return 0;

		return parsedBuild;
	}
}

namespace Memory::FiveMCompat
{
	bool IsFiveM()
	{
		return std::any_of(g_FiveMModules.begin(), g_FiveMModules.end(), [](auto module) {
			return GetModuleHandle(module) != nullptr;
		});
	}

	ClientBuild DetectClientBuild()
	{
		if (!IsFiveM())
			return ClientBuild::Unknown;

		// FiveM does not expose an official build API through SHV, so reuse the game build string extracted
		// by pattern scan and map it to known supported FiveM client builds.
		switch (ParseBuildNumber(Memory::GetGameBuild()))
		{
		case 3717:
			return ClientBuild::Build3717;
		case 3757:
			return ClientBuild::Build3757;
		default:
			return ClientBuild::Unknown;
		}
	}

	bool IsSupportedClient(ClientBuild clientBuild)
	{
		return clientBuild == ClientBuild::Build3717 || clientBuild == ClientBuild::Build3757;
	}

	const char *ToString(ClientBuild clientBuild)
	{
		switch (clientBuild)
		{
		case ClientBuild::Build3717:
			return "3717";
		case ClientBuild::Build3757:
			return "3757";
		default:
			return "unknown";
		}
	}


	const NativeOffsets &GetNativeOffsets(ClientBuild clientBuild)
	{
		switch (clientBuild)
		{
		case ClientBuild::Build3717:
			return g_Build3717Offsets;
		case ClientBuild::Build3757:
			return g_Build3757Offsets;
		default:
			return g_DefaultOffsets;
		}
	}

	void LogCompatibilityStatus()
	{
		if (!IsFiveM())
		{
			LOG("Running on GTA V / ScriptHookV host (FiveM modules not detected)");
			return;
		}

		auto clientBuild = DetectClientBuild();
		if (IsSupportedClient(clientBuild))
		{
			const auto &offsets = GetNativeOffsets(clientBuild);
			LOG("Compatible client FiveM " << ToString(clientBuild)
			                               << " detected. Enabling compatible native wrappers and guarded hooks.");
			LOG("FiveM native offset profile: WAIT=" << offsets.WaitHandlerDelta
			                                      << ", SCREEN_FADE=" << offsets.ScreenFadeHandlerDelta
			                                      << ", SET_ENTITY_HEALTH args=" << offsets.SetEntityHealthArgCount);
		}
		else
		{
			LOG("Warning: FiveM detected but client build is not in the validated list (3717/3757). "
			    "Falling back to conservative compatibility mode.");
		}
	}
}
