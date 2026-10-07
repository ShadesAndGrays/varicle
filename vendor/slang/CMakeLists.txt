# cmake_minimum_required(VERSION 3.22)

include(FetchContent)

set(SLANG_VERSION "2026.19") # pin version
set(WIN_VER_HASH "fc922f214b5d115f663932e59e80e0f6b6cc6fae140979cf307da6459b6f8f33") # pin version
# set(LINUX_VER_HASH "758446aa36dd9dc985b6a93e79fff9ed7fc139180c89cb5d3f09427be5d0d2b7") # pin version
# https://github.com/shader-slang/slang/releases/download/v2026.19/slang-2026.19-windows-x86_64.zip


if(WIN32)
    set(PLATFORM "windows-x86_64")
    set(EXT "zip")
    set(SLANG_HASH ${WIN_VER_HASH})
elseif(APPLE)
  set(_plat "macos-aarch64")   # or macos-x86_64
  set(_ext "zip")
else()
  set(_plat "linux-x86_64")
  set(_ext "tar.gz")
endif()

FetchContent_Declare( slang_prebuilt
    URL https://github.com/shader-slang/slang/releases/download/v${SLANG_VERSION}/slang-${SLANG_VERSION}-${PLATFORM}.${EXT}
    URL_HASH SHA256=${SLANG_HASH}
)

FetchContent_MakeAvailable(slang_prebuilt)

set(_root ${slang_prebuilt_SOURCE_DIR} )
file(GLOB _slang_dlls ${_root}/bin/*.dll)

set(SLANG_ROOT "${slang_prebuilt_SOURCE_DIR}" PARENT_SCOPE)
set(SLANG_RUNTIME_DLLS "${_slang_dlls}" PARENT_SCOPE)


message(STATUS "slang root = ${_root}")

add_library(slang SHARED IMPORTED GLOBAL)

if(WIN32)
    set_target_properties( slang PROPERTIES
        IMPORTED_IMPLIB ${_root}/lib/slang.lib
        IMPORTED_LOCATION ${_root}/bin/slang.dll
    )
elseif(APPLE)
  set_target_properties(slang PROPERTIES IMPORTED_LOCATION ${_root}/lib/libslang.dylib)
else()
  set_target_properties(slang PROPERTIES IMPORTED_LOCATION ${_root}/lib/libslang.so)
endif()


