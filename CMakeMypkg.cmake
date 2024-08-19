include(FetchContent)

FetchContent_Declare(
  ZxFile
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxFile.git
  GIT_TAG 8a91c2f97de9ab56abffd1ba4ecd84036932b129)

FetchContent_MakeAvailable(ZxFile)

FetchContent_Declare(
  ZxMem
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxMem.git
  GIT_TAG 2c66243e752c7ff8a3d84cb52fec8a69eb21949b)

FetchContent_MakeAvailable(ZxMem)

FetchContent_Declare(
  ZxFS
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxFS.git
  GIT_TAG 5dcf8441c05c9f981950e2585735e5c0745cf8a2)

FetchContent_MakeAvailable(ZxFS)

if(WIN32)
    set(ZLIB_BUILD_EXAMPLES OFF)
    FetchContent_Declare(
        zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG v1.3.1)

    FetchContent_MakeAvailable(zlib)

    if(BUILD_SHARED_LIBS)
        add_library(ZLIB::ZLIB ALIAS zlib)
    else()
        add_library(ZLIB::ZLIB ALIAS zlibstatic)
    endif()
    
elseif(UNIX)
    find_package(ZLIB REQUIRED)
endif()


if((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND MSVC)
    FetchContent_Declare(
        ZxHook
        GIT_REPOSITORY https://github.com/ZQF-Zut/ZxHook.git
        GIT_TAG 2a62970fc8df8ae4102af05c35efd1d539dff7ba)

    FetchContent_MakeAvailable(ZxHook)
endif()
