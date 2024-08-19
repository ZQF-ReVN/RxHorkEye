# Value Define
set(PROJECT_NAME HanaHime_cn)

# Cpp Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add Library
link_libraries(HorkEye)

# Current Project
project("${PROJECT_NAME}")
add_library(${PROJECT_NAME} MODULE "dllmain.cpp")
