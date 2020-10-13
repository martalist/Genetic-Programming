#include <gtest/gtest.h>
#include "../src/model/FunctionFactory.h"
#include "../src/Population.h"

namespace
{
    const Model::PopulationParams Params1
    {
        2, // population size
        10, // The minimum tree size of newly (randomly) created S-expressions
        0.7, // crossover probability
        0.001, // mutation probability
        0.001, // hoist mutation probability
        { Model::FunctionType::SquareRoot }, // allowed functions
        1, // The number of terminals
        0, // random seed
        false // always replace parents?
    };

    const Model::PopulationParams Params2
    {
        4, // population size
        10, // The minimum tree size of newly (randomly) created S-expressions
        0.7, // crossover probability
        0.001, // mutation probability
        0.001, // hoist mutation probability
        { Model::FunctionType::Addition, Model::FunctionType::Multiplication, Model::FunctionType::SquareRoot }, // allowed functions
        2, // The number of terminals
        0, // random seed
        false // always replace parents?
    };

    const std::vector<std::vector<double>> FitnessCases1
    {
        { 3.0,  5.830952 },
        { 8.0,  16.124515 },
        { 18.0, 18.11077 },
        { 32.0, 33.837849 },
        { 4.0,  5.0 },
        { 12.0, 15.620499 },
        { 21.0, 21.84033 },
        { 7.0,  8.062258 },
        { 16.0, 28.84441 },
        { 2.0,  9.219545 }
    };

    const std::vector<std::vector<double>> FitnessCases2
    {
        { 3.0,  5.0,  5.830952 },
        { 8.0,  14.0, 16.124515 },
        { 18.0, 2.0,  18.11077 },
        { 32.0, 11.0, 33.837849 },
        { 4.0,  3.0,  5.0 },
        { 12.0, 10.0, 15.620499 },
        { 21.0, 6.0,  21.84033 },
        { 7.0,  4.0,  8.062258 },
        { 16.0, 24.0, 28.84441 },
        { 2.0,  9.0,  9.219545 }
    };
}

namespace Tests
{
    using namespace Model;

    class PopulationTest : public ::testing::Test
    {
    public:
        const std::vector<Population::Chromosome>& AccessPopulation(const Population& p) { return p.m_population; }
    protected:
        PopulationTest() { }

        ~PopulationTest() = default;

        Population p1{Params1, FitnessCases1};
        Population p2{Params2, FitnessCases2};
    };

    TEST_F(PopulationTest, ChromosomeConstructor) 
    {
        // population is empty until Reset
        ASSERT_EQ(0, AccessPopulation(p1).size());
        p1.Reset();

        // All chromosomes in p1 are guaranteed to be (sqrt a)
        ASSERT_DOUBLE_EQ(12.992451294754199, AccessPopulation(p1)[0].Fitness);
    }

    TEST_F(PopulationTest, ChromosomeFitness) 
    {
        // test CalculateFitness and CalculateWeightedFitness
    }

    TEST_F(PopulationTest, ChromosomeOperatorLessThan) 
    {
        p2.Reset();
        // test that operator< works properly
        const auto& c1 = AccessPopulation(p2)[0];
        const auto& c2 = AccessPopulation(p2)[1];

        // Less than operator includes parsimony coefficient
        ASSERT_EQ(c1.WeightedFitness < c2.WeightedFitness, c1 < c2);
        ASSERT_EQ(c2.WeightedFitness < c1.WeightedFitness, c2 < c1);
        ASSERT_FALSE(c2.WeightedFitness < c2.WeightedFitness);
    }

    TEST_F(PopulationTest, PopulationConstructor) 
    {
        ASSERT_EQ(0, AccessPopulation(p1).size());
        ASSERT_EQ(0, AccessPopulation(p2).size());
        p1.Reset();
        p2.Reset();

        ASSERT_EQ(Params1.PopulationSize, AccessPopulation(p1).size());
        ASSERT_EQ(Params2.PopulationSize, AccessPopulation(p2).size());
    }

    TEST_F(PopulationTest, PopulationReset) 
    {
        // p2.Reset();
        // auto originalP2(AccessPopulation(p2));
        // p2.Reset();
        // ASSERT_FALSE(std::equal(originalP2.begin(), originalP2.end(), AccessPopulation(p2).begin()));
    }

    TEST_F(PopulationTest, PopulationEvolve) 
    {
        // test that the new population is correct, as per the stored params
    }

    TEST_F(PopulationTest, PopulationAverageFitness) 
    {
        // test the method correctly returns the average
    }

    TEST_F(PopulationTest, PopulationBestFitness) 
    {
        // test the method correctly returns the best
        // for 0, 1, more chromosomes
        p1.Reset();
        auto [ min, qrt1, median, qrt3, max ] = p1.GetRangeStatistics();
        ASSERT_DOUBLE_EQ(12.992451294754199, min);

        // need to test after a couple of evolutions
    }

    TEST_F(PopulationTest, PopulationSortPopulation) 
    {
        // test the method correctly returns the best
        // for 0, 1, more chromosomes
    }

    TEST_F(PopulationTest, PopulationGetNewOffspring) 
    {
        // Check that mutation and crossover occur when probability is = 1.0
        // and don't happen when probability is 0.0
    }

    TEST_F(PopulationTest, PopulationParsimonyCoefficient) 
    {
        // Check that mutation and crossover occur when probability is = 1.0
        // and don't happen when probability is 0.0
    }
}
