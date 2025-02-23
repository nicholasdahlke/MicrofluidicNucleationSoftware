cmake_minimum_required(VERSION 3.15)

project(
        MicrofluidicNucleationSoftware
        VERSION 0.1
        DESCRIPTION "A software to evaluate nucleation processes measured using microfluidic droplets."
        LANGUAGES CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

add_subdirectory(src)
add_subdirectory(apps)