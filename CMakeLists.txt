cmake_minimum_required(VERSION 3.16)

project(hh-pvdata)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

if (CMAKE_BUILD_TYPE MATCHES Debug)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif ()

if (CMAKE_BUILD_TYPE MATCHES Release)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(hh-pvdata
    src/main.cpp
    src/pvdata.cpp
    src/pvdata-cli.cpp)
