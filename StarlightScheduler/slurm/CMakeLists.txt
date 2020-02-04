cmake_minimum_required(VERSION 3.14)
project(slurm)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib64)
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/bin)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib64)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
            -g
            -D_DEBUG
    )
else()
    add_compile_options(-O2)
endif()

set(SOURCES src/slurm_wrapper.c)

add_library(slurm_wrapper ${SOURCES})
add_executable(test src/test.c)
target_link_libraries(test slurm_wrapper slurm)