#include <gtest/gtest.h>
#include "RaffleTest.cpp"
#include "FunctionTest.cpp"
#include "OperatorsTest.cpp"
#include "PopulationTest.cpp"
#include "MathTest.cpp"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

