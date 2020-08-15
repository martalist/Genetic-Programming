# This is the base CMakeLists file

# give the project a name
project(Genetic_Programming)
cmake_minimum_required(VERSION 3.0) # and a version
SET(CMAKE_CPP_COMPILER /usr/bin/g++) # specify the compiler
SET(CMAKE_CXX_FLAGS "--std=c++17 -Wall")

# message(STATUS "!!!!!!!!!!!!! ${PROJECT_SOURCE_DIR}") # print a msg

include_directories(${PROJECT_SOURCE_DIR}/src) # tells cmake that all headers should be searched for, starting at this folder.
include_directories(${PROJECT_SOURCE_DIR}/src/utils) # tells cmake that all headers should be searched for, starting at this folder.
add_subdirectory(${PROJECT_SOURCE_DIR}/src) 

# Must be defined in top-most CMakeLists.txt
enable_testing() 

# Outsource tests to another folder
add_subdirectory(${PROJECT_SOURCE_DIR}/tests) 
