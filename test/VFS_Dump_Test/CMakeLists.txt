# Project Name
project(ReVN_RxHorkEye_VFS_Dump_Test)

# Test Project
add_library("${PROJECT_NAME}" MODULE "dllmain.cpp")
target_compile_options("${PROJECT_NAME}" PRIVATE /W4)
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_23)
target_compile_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Add Library
target_link_libraries("${PROJECT_NAME}" PRIVATE ReVN::RxHorkEye::Hook)
