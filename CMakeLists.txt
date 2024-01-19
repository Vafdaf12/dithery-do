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

add_executable(${NAME_EXE}
    external/stb/stb_image.cpp
    external/stb/stb_image_write.cpp

    src/main.cpp
    src/color/LabColorSpace.cpp
    src/color/XyzColorSpace.cpp
    src/select/ClosestEuclidian.cpp
    src/select/ClosestLine.cpp
    src/select/PartitionBlend.cpp
    src/pipeline/XyzConversion.cpp
    src/pipeline/LabConversion.cpp
    src/pipeline/PipelineStep.cpp
    src/Palette.cpp
)
target_include_directories(${NAME_EXE}
    PRIVATE external
    PRIVATE src
)

target_link_libraries(${NAME_EXE} argparse glm)


