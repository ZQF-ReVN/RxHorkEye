# RxHorkEye
cmake_minimum_required(VERSION 3.25)

# Project Name
project(ReVN_RxHorkEye)

# Export Symbols
if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Set Source File Path
list(APPEND ReVN_RxHorkEye_Core_Src_Files
    "Core/RxHorkEye/ARC.cpp"
    "Core/RxHorkEye/Cryptor.cpp"
    "Core/RxHorkEye/HashStrTable.cpp")

# Set Library
add_library(ReVN_RxHorkEye_Core ${ReVN_RxHorkEye_Core_Src_Files})
add_library(ReVN::RxHorkEye::Core ALIAS ReVN_RxHorkEye_Core)
target_include_directories(ReVN_RxHorkEye_Core PUBLIC "Core")
target_compile_features(ReVN_RxHorkEye_Core PRIVATE cxx_std_23)
target_compile_options(ReVN_RxHorkEye_Core PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
target_link_libraries(ReVN_RxHorkEye_Core PRIVATE ZLIB::ZLIB)
target_link_libraries(ReVN_RxHorkEye_Core PUBLIC Zut::ZxMem Zut::ZxFile Zut::ZxFS)

# Warning
if(MSVC)
    target_compile_options(ReVN_RxHorkEye_Core PRIVATE /W4)
else()
    target_compile_options(ReVN_RxHorkEye_Core PRIVATE -Wall -Wextra)
endif()


# Hook
# if((MSVC) AND (CMAKE_SIZEOF_VOID_P EQUAL 4))
#     # Set Source File Path
#     list(APPEND ReVN_RxHorkEye_Hook_Src_Files
#         "Hook/RxHorkEye/VFS.cpp")
# 
#     # Set Library
#     add_library(ReVN_RxHorkEye_Hook ${ReVN_RxHorkEye_Hook_Src_Files})
#     add_library(ReVN::RxHorkEye::Hook ALIAS ReVN_RxHorkEye_Hook)
#     target_include_directories(ReVN_RxHorkEye_Hook PUBLIC "Hook")
#     target_compile_options(ReVN_RxHorkEye_Hook PRIVATE /W3)
#     target_compile_features(ReVN_RxHorkEye_Hook PRIVATE cxx_std_23)
#     target_compile_options(ReVN_RxHorkEye_Hook PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
#     target_link_libraries(ReVN_RxHorkEye_Hook PRIVATE Zut::ZxHook)
# endif()
