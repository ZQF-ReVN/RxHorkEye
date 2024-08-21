add_subdirectory("Pack_Test")

# Only MSVC x32
if((MSVC) AND (CMAKE_SIZEOF_VOID_P EQUAL 4))
    add_subdirectory("VFS_Dump_Test")
endif()
