# Main CMakeLists
cmake_minimum_required(VERSION 3.12)

# Project Name
project(ACV1Tools)

# MSVC Setting
if(MSVC)
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# Value Define
set(LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/lib")
set(THIRD_DIR "${CMAKE_CURRENT_LIST_DIR}/third")

# Out Path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/.out/exe)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/.out/lib)

# Library
     add_subdirectory("lib/ACV")
     add_subdirectory("lib/Rut")
     add_subdirectory("lib/RxHook")

# Project
    add_subdirectory("src/VFSHook")
    add_subdirectory("src/VFSDump")
    add_subdirectory("src/VFSExtract")
