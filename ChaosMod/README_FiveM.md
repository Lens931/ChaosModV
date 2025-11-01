# ChaosMod FiveM Fiber Safety Notes

This build of ChaosMod is intended to run inside the ScriptHookV fiber created by FiveM.  The following configuration flags and coding guidelines are required to avoid the CRT fiber-safety crash (`MSVCP140.dll!mtx_do_lock`):

* Compile with `/Zc:threadSafeInit-` so the CRT does not inject its mutex when initializing function-local statics.
* Use the dynamic runtime (`/MD`) via `CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL`.
* Define `NOMINMAX`, `WIN32_LEAN_AND_MEAN`, and `_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR` for all translation units.
* Prefer Win32 SRW locks or atomics (`Interlocked*`) instead of STL synchronisation primitives such as `std::mutex`, `std::recursive_mutex`, or `std::call_once`.
* Perform expensive initialization inside the script fiber (e.g. in `scriptRegister` callbacks) rather than in `DllMain`.
* Avoid Structured Exception Handling blocks inside fiber entry points—wrap tick processing in standard C++ `try`/`catch` if required.

For additional implementation details refer to `include/Sync/Lock.h` (SRW lock helpers) and `include/Init/Once.h` (spin-based init helper).
