#pragma once

#include <string>

namespace Memory::FiveMCompat
{
	// Explicit list of FiveM client builds the low-level integration layer was validated against.
	enum class ClientBuild
	{
		Unknown = 0,
		Build3717,
		Build3757,
	};

	struct NativeOffsets
	{
		int WaitHandlerDelta        = 0;
		int ScreenFadeHandlerDelta  = 0;
		int SetEntityHealthArgCount = 3;
	};

	bool IsFiveM();
	ClientBuild DetectClientBuild();
	bool IsSupportedClient(ClientBuild clientBuild);
	const char *ToString(ClientBuild clientBuild);
	const NativeOffsets &GetNativeOffsets(ClientBuild clientBuild);
	void LogCompatibilityStatus();
}

