cmake_minimum_required(VERSION 3.16)

project(mindshift-backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem thread)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(src)
