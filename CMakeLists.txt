cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

project(ffmpeg_with_vcpkg CXX)

find_package(ffmpeg  REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(x main.cpp )
target_link_libraries(x PRIVATE fmt::fmt)
target_include_directories(x PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(x PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(x PRIVATE ${FFMPEG_LIBRARIES})


