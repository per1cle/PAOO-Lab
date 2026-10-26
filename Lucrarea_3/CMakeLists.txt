cmake_minimum_required(VERSION 3.10)
project(lab3_paoo)

# Setăm standardul C++ la cel puțin C++14 (recomandat și pentru move semantics)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Adăugăm folderul inc/ la căile de căutare pentru headere
include_directories(inc)

# Definim executabilul și fișierele asociate proiectului
add_executable(${PROJECT_NAME}
               inc/Student.hpp
               src/Student.cpp
               src/main.cpp)
