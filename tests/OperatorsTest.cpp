#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include "../src/model/Chromosome.h"
#include "../src/model/Operators.h"
#include "../src/model/Terminal.h"

namespace
{
    double A = 1.5;
    double B = 3.3;
}

namespace Tests
{
    using namespace Model;

    TEST(OperatorsTest, MutationOneFunction)
    {
        Chromosome func { FunctionFactory::Create(FunctionType::Addition) };
        std::vector<FunctionType> allowedFunctions{ FunctionType::Subtraction };
        std::vector<double*> allowedTerminals{};

        // since there is only one node, and only one allowed function,
        // the mutation should change this to a sutraction
        func.Mutate(allowedFunctions, allowedTerminals);
        func.GetTree()->AddChild(std::make_unique<Terminal>(&B));
        func.GetTree()->AddChild(std::make_unique<Terminal>(&A));
        ASSERT_DOUBLE_EQ(B-A, func.GetTree()->Evaluate());
    }

    TEST(OperatorsTest, DontMutateWithOnlyOneTerminal)
    {
        Chromosome var { FunctionFactory::Create(&A) };
        std::vector<FunctionType> allowedFunctions{};
        std::vector<double*> allowedTerminals{ &A };

        // since there is only one node (Terminal), and only one allowed terminal,
        // the mutation should change this to B.
        var.Mutate(allowedFunctions, allowedTerminals);
        ASSERT_DOUBLE_EQ(A, var.GetTree()->Evaluate());
    }

    TEST(OperatorsTest, MutationToOtherTerminal)
    {
        Chromosome var { FunctionFactory::Create(&A) };
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
        Chromosome func { FunctionFactory::Create(FunctionType::Addition) };
        func.GetTree()->AddChild(FunctionFactory::Create(&four));
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
        Chromosome root { FunctionFactory::Create(FunctionType::SquareRoot) };
        auto func = FunctionFactory::Create(FunctionType::SquareRoot);
        func->AddChild(FunctionFactory::Create(&A));
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
        std::vector<FunctionType> allowedFunctions{ FunctionType::Addition };
        std::vector<double*> allowedTerminals{ &A };
        auto chromosome = Operators::CreateRandomChromosome(1, allowedFunctions, allowedTerminals);

        // std::cout << chromosome->ToString() << std::endl;
        ASSERT_DOUBLE_EQ(A+A, chromosome->Evaluate());
    }

    TEST(OperatorsTest, CreateRandomChromosome)
    {
        std::vector<FunctionType> allowedFunctions{ FunctionType::Addition };
        std::vector<double*> allowedTerminals{ &A, &B };
        auto chromosome = Operators::CreateRandomChromosome(3, allowedFunctions, allowedTerminals);

        // std::cout << chromosome->ToString() << std::endl;
        // ASSERT_DOUBLE_EQ(A, chromosome->Evaluate());
    }
}
