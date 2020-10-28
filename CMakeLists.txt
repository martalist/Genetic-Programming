# This is the base CMakeLists file

# give the project a name
cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.0) # and a version
project(gpforecast VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CPP_COMPILER /usr/bin/g++) # specify the compiler
include(GNUInstallDirs)

if (APPLE)
    set(CMAKE_CXX_FLAGS "-Wall")
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_FLAGS "--std=c++17 -Wall")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
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

# Eigen for matrix algebra
find_package(Eigen3 3.3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

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

# copy the config file to the binary dir
configure_file(${PROJECT_SOURCE_DIR}/src/config.xml config.xml COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/pythagorean_theorem.csv pythagorean_theorem.csv COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/src/HotelRooms.csv HotelRooms.csv COPYONLY)

# create a shared library
# based on https://stackoverflow.com/a/45843676/5200859
add_library(${PROJECT_NAME} SHARED src/${PROJECT_NAME}.cpp)
target_link_libraries(${PROJECT_NAME}
    prog
    model
    Eigen3::Eigen
    ${Boost_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/${PROJECT_NAME}.h)

target_include_directories(${PROJECT_NAME} PRIVATE include)

configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

# Must be defined in top-most CMakeLists.txt
enable_testing() 
# include test directory
add_subdirectory(${PROJECT_SOURCE_DIR}/tests) 
