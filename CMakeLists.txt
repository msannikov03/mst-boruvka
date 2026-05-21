cmake_minimum_required(VERSION 3.20)
project(mst_boruvka LANGUAGES CXX)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

add_library(mst_warnings INTERFACE)
target_compile_options(mst_warnings INTERFACE
    -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-sign-conversion
)

if(APPLE)
    foreach(_candidate "/opt/homebrew/opt/libomp" "/usr/local/opt/libomp")
        if(EXISTS "${_candidate}")
            set(_libomp_root "${_candidate}")
            break()
        endif()
    endforeach()
    if(_libomp_root)
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${_libomp_root}/include" CACHE STRING "" FORCE)
        set(OpenMP_CXX_LIB_NAMES "omp" CACHE STRING "" FORCE)
        set(OpenMP_omp_LIBRARY "${_libomp_root}/lib/libomp.dylib" CACHE FILEPATH "" FORCE)
    endif()
endif()
find_package(OpenMP REQUIRED)

find_package(Boost REQUIRED COMPONENTS graph)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    URL_HASH SHA256=1f357c27ca988c3f7c6b4bf68a9395005ac6761f034046e9dde0896e3aba00e4
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(mst INTERFACE)
target_include_directories(mst INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(mst INTERFACE OpenMP::OpenMP_CXX)

add_subdirectory(tests)
