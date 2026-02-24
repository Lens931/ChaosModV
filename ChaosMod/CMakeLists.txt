cmake_minimum_required(VERSION 3.31)
project(ChaosMod)

option(CHAOS_ENABLE_FIVEM_37XX "Enable FiveM 3717/3757 compatibility gates" ON)

file(GLOB ROOT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/Components/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Effects/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Memory/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Util/*.cpp)
file(GLOB PATTERNS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/Patterns/Patterns.cpp)
add_library(ChaosMod MODULE ${ROOT_SRC} ${SRC} ${PATTERNS_SRC} ChaosMod.rc)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # Ensure every target (including dependencies) links against the dynamic runtime for
    # FiveM fiber compatibility. The value must be set before adding subdirectories so the
    # vendor projects inherit it.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif ()

file (STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../version.txt MOD_VERSION)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/InfoTEMPLATE.h.in ${CMAKE_CURRENT_SOURCE_DIR}/Info.h @ONLY)

target_precompile_headers(ChaosMod PUBLIC stdafx.cpp)

set_target_properties(ChaosMod PROPERTIES SUFFIX ".asi")
set_target_properties(ChaosMod PROPERTIES CXX_STANDARD 20)
set_target_properties(ChaosMod PROPERTIES CXX_SCAN_FOR_MODULES OFF)

target_compile_definitions(ChaosMod PUBLIC $<$<CONFIG:Debug>:CHAOSDEBUG> NDEBUG WIN32_LEAN_AND_MEAN NOMINMAX _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR SOL_ALL_SAFETIES_ON SOL_DEFAULT_PASS_ON_ERROR)

if (CHAOS_ENABLE_FIVEM_37XX)
    target_compile_definitions(ChaosMod PUBLIC CHAOS_ENABLE_FIVEM_37XX=1)
endif ()

set(include_dirs ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../vendor ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/lua/include)
set(link_libs shv minhook lua54 winmm d3dcompiler xinput)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/shv shv)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/minhook minhook)
if (WITH_DEBUG_PANEL_SUPPORT)
    set(include_dirs ${include_dirs} ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/IXWebSocket)
    set(link_libs ${link_libs} wsock32 ws2_32 ixwebsocket)

    target_compile_definitions(ChaosMod PUBLIC WITH_DEBUG_PANEL_SUPPORT)

    set(USE_ZLIB OFF CACHE BOOL "Enable zlib support")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/IXWebSocket ixwebsocket)
endif ()

target_include_directories(ChaosMod PUBLIC ${include_dirs})
target_link_directories(ChaosMod PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/lua)
target_link_libraries(ChaosMod PUBLIC ${link_libs})

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(ChaosMod PUBLIC /Zi /GT /W2 /WX /bigobj /Zc:threadSafeInit- /EHsc -DUNICODE -D_UNICODE -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(ChaosMod PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)
    target_link_options(ChaosMod PUBLIC /DEBUG /OPT:REF /OPT:ICF)
else()
    target_compile_options(ChaosMod PUBLIC -municode -Wa,-mbig-obj -flarge-source-files -Werror -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wno-unused-variable -Wno-maybe-uninitialized -Wno-array-bounds -Wno-unused-but-set-variable)
    target_link_options(ChaosMod PUBLIC -static)
endif()
