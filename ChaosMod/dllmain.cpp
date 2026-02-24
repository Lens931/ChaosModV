#include <stdafx.h>

#include "Info.h"
#include "Main.h"

#include "Memory/Memory.h"
#include "Memory/FiveMCompat.h"

#include "Util/CrashHandler.h"


static bool CanRegisterWithCurrentLoader()
{
	// Modern FiveM clients can host ASI plugins only when ASI loading is enabled. We cannot force-enable
	// that from inside the plugin, so only log capabilities and proceed conservatively.
	if (Memory::FiveMCompat::IsFiveM())
	{
		const auto clientBuild = Memory::FiveMCompat::DetectClientBuild();
		if (!Memory::FiveMCompat::IsSupportedClient(clientBuild))
		{
			LOG("Warning: FiveM ASI host detected with unsupported build "
			    << Memory::FiveMCompat::ToString(clientBuild)
			    << ". Script registration continues in compatibility mode.");
		}
		else
		{
			LOG("Compatible client FiveM " << Memory::FiveMCompat::ToString(clientBuild)
			                               << " detected during loader attach.");
		}
	}

	return true;
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	{
		SetUnhandledExceptionFilter(CrashHandler);

		RAW_LOG("Chaos Mod v" MOD_VERSION "\n\n");

		if (CanRegisterWithCurrentLoader())
		{
			scriptRegister(instance, Main::OnRun);
			keyboardHandlerRegister(Main::OnKeyboardInput);
		}

		break;
        }
        case DLL_PROCESS_DETACH:
                if (Main::IsMemoryInitialized())
                        Memory::Uninit();

                scriptUnregister(instance);

                keyboardHandlerUnregister(Main::OnKeyboardInput);

		if (GetConsoleWindow())
		{
			g_ConsoleOut.close();

			FreeConsole();
		}

		break;
	}

	return TRUE;
}
