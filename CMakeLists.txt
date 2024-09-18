cmake_minimum_required(VERSION 3.30)

project(xci_combiner)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCE_FILES
    source/main.cpp
    source/io.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
