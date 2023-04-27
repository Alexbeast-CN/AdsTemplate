cmake_minimum_required(VERSION 3.0.0)
project(AdsTest VERSION 0.1.0)

find_package(Threads)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(COMPILE_OPTIONS
    -pedantic
    -Wall
    -Wextra
  )
  add_definitions(-D_GNU_SOURCE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # Compiler flags and definitions for Visual Studio come here
endif()
add_definitions(-DCONFIG_DEFAULT_LOGLEVEL=1)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

include_directories(AdsLib)
add_subdirectory(AdsLib)

add_executable(example example.cpp)
target_link_libraries(example PUBLIC ads)


add_executable(ctrlTest ctrlTest.cpp)
target_link_libraries(ctrlTest PUBLIC ads)