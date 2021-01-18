cmake_minimum_required(VERSION 3.5)
include(FindPkgConfig)

project(main)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11 -pthread")
set(source_dir "${PROJECT_SOURCE_DIR}/src/")
add_definitions(-DBOOST_LOG_DYN_LINK)

file(GLOB source_files "${source_dir}/*.cpp")

find_package(OpenCV 4.4.0 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(Boost 1.67.0 REQUIRED COMPONENTS log_setup log)
include_directories(${Boost_INCLUDE_DIRS})
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
pkg_check_modules(SDL2_Mixer REQUIRED IMPORTED_TARGET SDL2_mixer)

add_executable(main ${source_files})
target_link_libraries(main ${OpenCV_LIBS})
target_link_libraries(main ${Boost_LIBRARIES})
target_link_libraries(main ${SDL2_LIBRARIES})
target_link_libraries(main PkgConfig::SDL2_Mixer)
target_link_libraries(main i2c)
