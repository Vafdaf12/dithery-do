cmake_minimum_required(VERSION 3.11)
project(DitheryDo)

set(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(NAME_EXE ${PROJECT_NAME})


# ------------ FETCH GLM ------------ 
FetchContent_Declare(glm 
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

# ------------ FETCH CLI11 ------------ 
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)


# ------------ CONFIGURE APPLICATION -------------------
file(GLOB SRCFILES
    src/*.cpp
    src/stb/*.cpp
    src/select/*.cpp
    src/color/*.cpp
)

add_executable(${NAME_EXE} ${SRCFILES})
target_link_libraries(${NAME_EXE} argparse glm)


