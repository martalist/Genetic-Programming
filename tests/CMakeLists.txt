# Add gtest sources folder. Provides gtest, gtest_main
add_subdirectory(/usr/src/gtest
                ${PROJECT_BINARY_DIR}/gtest)

include(CTest) # include testing cmake package.

# Set binary name for convenience
Set(TEST_BINARY ${PROJECT_NAME}_test)

# This is an executable that runs the tests.
add_executable(${TEST_BINARY} test_blah.cpp)

# Link the executable to needed libraries.
target_link_libraries(${TEST_BINARY}
    foobar               # Library we are testing
    gtest gtest_main   # GTest libraries
)

# Add gtest to be able to run ctest
add_test(NAME ${TEST_BINARY}
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/${TEST_BINARY})
