#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include "../src/PopulationParams.h"
#include "../src/model/NormalChromosome.h"
#include "../src/model/FunctionFactory.h"
#include "../src/utils/UniformRandomGenerator.h"

namespace
{
    double A = 1.5;
    double B = 3.3;
    Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>> RandInt(0, 1);
}

namespace Tests
{
    using namespace Model;

    TEST(OperatorsTest, MutationOneFunction)
    {
        FunctionFactory factory;
        NormalChromosome func { factory.Create(FunctionType::Addition), RandInt, factory};
        std::vector<FunctionType> allowedFunctions{ FunctionType::Subtraction };
        std::vector<double*> allowedTerminals{};

        // since there is only one node, and only one allowed function,
        // the mutation should change this to a sutraction
        func.Mutate(allowedFunctions, allowedTerminals);
        func.GetTree()->AddChild(factory.Create(&B));
        func.GetTree()->AddChild(factory.Create(&A));
        ASSERT_DOUBLE_EQ(B-A, func.GetTree()->Evaluate());
    }

    TEST(OperatorsTest, DontMutateWithOnlyOneTerminal)
    {
        FunctionFactory factory;
        NormalChromosome var { factory.Create(&A), RandInt, factory };
        std::vector<FunctionType> allowedFunctions{};
        std::vector<double*> allowedTerminals{ &A };

        // since there is only one node (Terminal), and only one allowed terminal,
        // the mutation should change this to B.
        var.Mutate(allowedFunctions, allowedTerminals);
        ASSERT_DOUBLE_EQ(A, var.GetTree()->Evaluate());
    }

    TEST(OperatorsTest, MutationToOtherTerminal)
    {
        FunctionFactory factory;
        NormalChromosome var { factory.Create(&A), RandInt, factory };
        std::vector<FunctionType> allowedFunctions{};
        std::vector<double*> allowedTerminals{ &A, &B };

        // since there is only one node (Terminal), and only one allowed terminal,
        // the mutation should change this to B.
        var.Mutate(allowedFunctions, allowedTerminals);
        ASSERT_DOUBLE_EQ(B, var.GetTree()->Evaluate());
    }

    TEST(OperatorsTest, MutateEitherTerminalOrFunction)
    {
        double two = 2.0;
        double four = 4.0;
        FunctionFactory factory;
        NormalChromosome func { factory.Create(FunctionType::Addition), RandInt, factory };
        func.GetTree()->AddChild(factory.Create(&four));
        std::vector<FunctionType> allowedFunctions{ FunctionType::SquareRoot };
        std::vector<double*> allowedTerminals{ &two };
        func.Mutate(allowedFunctions, allowedTerminals);

        if (func.Size() > 2)
        {
            ASSERT_DOUBLE_EQ(std::sqrt(2), func.GetTree()->Evaluate());
        }
        else
        {
            ASSERT_DOUBLE_EQ(two, func.GetTree()->Evaluate());
        }
    }

    TEST(OperatorsTest, HoistMutate)
    {
        FunctionFactory factory;
        NormalChromosome root { factory.Create(FunctionType::SquareRoot), RandInt, factory };
        auto func = factory.Create(FunctionType::SquareRoot);
        func->AddChild(factory.Create(&A));
        root.GetTree()->AddChild(std::move(func));

        while (root.Size() > 1)
        {
            root.HoistMutate();
            switch(root.Size())
            {
            case 3:
                ASSERT_DOUBLE_EQ(std::sqrt(std::sqrt(A)), root.GetTree()->Evaluate());
                break;
            case 2:
                ASSERT_DOUBLE_EQ(std::sqrt(A), root.GetTree()->Evaluate());
                break;
            default:
                ASSERT_DOUBLE_EQ(A, root.GetTree()->Evaluate());
                break;
            }
        }
    }

    TEST(OperatorsTest, TestFunctionThatCantHoldAllChildren)
    {
        // TODO: what happens when none of the allowed functions can
        // hold the number of children of the selected node?

        // TODO: Test that when there are 2 allowed functions, and one can't
        // hold the number of children, the other one is always selected.
    }

    TEST(OperatorsTest, MutateCompositeTree)
    {
        // TODO: do a more complicated mutation test
    }

    TEST(OperatorsTest, CreateSingleRandomChromosome)
    {
        FunctionFactory factory;
        std::vector<FunctionType> allowedFunctions{ FunctionType::Addition };
        std::vector<double*> allowedTerminals{ &A };
        double fitnessCases[] = { 1.0, 2.0, 3.0 };
        TrainingData fc{ fitnessCases, 3 };
        std::vector<double> terminals{ A };
        NormalChromosome chromosome{1, allowedFunctions, allowedTerminals, fc, terminals, 0.0, RandInt, factory};

        // std::cout << chromosome->ToString() << std::endl;
        ASSERT_DOUBLE_EQ(A+A, chromosome.GetTree()->Evaluate());
    }

    TEST(OperatorsTest, CreateRandomChromosome)
    {
        // std::vector<FunctionType> allowedFunctions{ FunctionType::Addition };
        // std::vector<double*> allowedTerminals{ &A, &B };

        // std::cout << chromosome->ToString() << std::endl;
        // ASSERT_DOUBLE_EQ(A, chromosome->Evaluate());
    }
}
