# This is the base CMakeLists file

# give the project a name
project(Integenetics)
cmake_minimum_required(VERSION 3.0) # and a version
SET(CMAKE_CPP_COMPILER /usr/bin/g++) # specify the compiler

if (APPLE)
SET(CMAKE_CXX_FLAGS "-Wall")
SET(CMAKE_CXX_STANDARD 17)
else()
SET(CMAKE_CXX_FLAGS "--std=c++17 -Wall")
endif()

# Boost libraries
set(Boost_USE_STATIC_LIBS ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS ON) # only find release libs
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.66.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "Could not find Boost")
endif()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
# if (CMAKE_VERSION VERSION_LESS 2.8.11)
  # include_directories("${gtest_SOURCE_DIR}/include")
# endif()

# include these source directories
add_subdirectory(${PROJECT_SOURCE_DIR}/src) 

# Must be defined in top-most CMakeLists.txt
enable_testing() 
# include test directory
add_subdirectory(${PROJECT_SOURCE_DIR}/tests) 
