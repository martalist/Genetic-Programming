#include <gtest/gtest.h>
#include <memory>
#include "../src/Population.h"

namespace Tests
{
    using namespace Model;

    class PopulationTest : public ::testing::Test
    {
    protected:
        PopulationTest() { }

        ~PopulationTest() = default;
    };

    TEST_F(PopulationTest, Blank) 
    {
        EXPECT_EQ(true, true);
    }
}
