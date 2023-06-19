cmake_minimum_required(VERSION 3.11)
project(DitheryDo)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_C_FLAGS_DEBUG "-O0")

set(NAME_EXE ${PROJECT_NAME})


# ------------ FETCH GLM ------------ 
FetchContent_Declare(glm 
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# ------------ FETCH CLI11 ------------ 
FetchContent_Declare(CLI11 
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(CLI11)


# ------------ CONFIGURE APPLICATION -------------------
file(GLOB SRCFILES
    src/*.cpp
    src/stb/*.cpp
    src/select/*.cpp
    src/color/*.cpp
)

add_executable(${NAME_EXE} ${SRCFILES})
target_link_libraries(${NAME_EXE} CLI11::CLI11 glm)


