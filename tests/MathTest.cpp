#include <gtest/gtest.h>
#include "../src/model/FunctionFactory.h"
#include "../src/utils/Math.h"

namespace
{
    const std::vector<int> X1 { 371498, 419640, 161128, 464626, 363964,
                           410801, 46430, 844336, 152548, 412759, 
                           491065, 94558, 692262, 32417, 929724, 
                           137163, 847900, 162916, 233859, 354702 };

    const std::vector<double> X2 { 662040.4705663299, 653135.0530801981, 313317.5910397772, 808457.842082715, 521698.1417861601, 
                            922811.6180404967, 495334.6789231453, 404982.1234716164, 759522.8722247136, 599136.8026392475, 
                            535476.7361361498, 169186.38271511678, 219023.63935516978, 758015.3875382886, 685665.7401744248, 
                            276089.60869302956, 885340.6994620715, 551290.1916048088, 691628.8542254892, 486923.94324422296 };
}
namespace Tests
{
    using namespace Util;
    using IntItr = std::vector<int>::const_iterator;
    using DblItr = std::vector<double>::const_iterator;

    class MathTest : public ::testing::Test
    {
    protected:
        MathTest() { }
        ~MathTest() = default;
    };

    TEST_F(MathTest, Average) 
    {
        double x1avg = Average<IntItr>(X1.begin(), X1.end(), [](const IntItr& itr) -> double { return *itr; });
        ASSERT_DOUBLE_EQ(381214.8, x1avg);

        double x2avg = Average<DblItr>(X2.begin(), X2.end(), [](const DblItr& itr) { return *itr; });
        ASSERT_DOUBLE_EQ(569953.9188501585, x2avg);
    }

    TEST_F(MathTest, Variance) 
    {
        double x1var = Variance<IntItr>(X1.begin(), X1.end(), [](const IntItr& itr) -> double { return *itr; });
        ASSERT_DOUBLE_EQ(73627594883, std::round(x1var));

        double x2var = Variance<DblItr>(X2.begin(), X2.end(), [](const DblItr& itr) { return *itr; });
        ASSERT_DOUBLE_EQ(46112533769, std::round(x2var));
    }

    TEST_F(MathTest, Covariance) 
    {
        double cov = Covariance<IntItr, DblItr>(X1.begin(), X1.end(), [](const IntItr& itr) -> double { return *itr; },
                X2.begin(), X2.end(), [](const DblItr& itr) { return *itr; });
        ASSERT_DOUBLE_EQ(10692913577, std::round(cov));
    }
}
