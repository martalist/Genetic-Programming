#include <gtest/gtest.h>
#include "RaffleTest.cpp"
#include "FunctionTest.cpp"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
