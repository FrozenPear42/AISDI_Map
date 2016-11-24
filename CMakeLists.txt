cmake_minimum_required(VERSION 2.6)
project(AISDI_Maps)

set(CMAKE_CXX_COMPILER g++-6)

include(CTest)
enable_testing()

include_directories("${PROJECT_SOURCE_DIR}/src")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 -Wall -pedantic -Wextra -Werror")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ")

add_subdirectory(src)
add_subdirectory(tests)
