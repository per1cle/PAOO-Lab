cmake_minimum_required(VERSION 3.10)
project(lab5_paoo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(inc)

add_executable(${PROJECT_NAME}
               inc/Car.hpp
               src/Car.cpp
               src/main.cpp)
