# Library Name
set(PROJECT_NAME ACV)

# Cpp Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add Library
link_libraries(Rut RxHook)

# Set Source File Path
list(APPEND SRC_FILE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ACV_VFS_Dump.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ACV_VFS_Hook.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ACV_VFS_Extract.cpp")

# Set Library
add_library(${PROJECT_NAME} "${SRC_FILE}")
