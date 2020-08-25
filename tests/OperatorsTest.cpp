#include <gtest/gtest.h>
#include <memory>
#include "../src/model/Operators.h"

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
        auto func = FunctionFactory::Create(FunctionType::Addition);
        std::vector<FunctionType> allowedFunctions{ FunctionType::Subtraction };
        std::vector<double*> allowedTerminals{};

        // since there is only one node, and only one allowed function,
        // the mutation should change this to a sutraction
        Operators::Mutate(func, allowedFunctions, allowedTerminals);
        func->AddChild(std::make_unique<Variable>(&B));
        func->AddChild(std::make_unique<Variable>(&A));
        ASSERT_DOUBLE_EQ(1.8, func->Evaluate());
    }

    TEST(OperatorsTest, MutationOneVariable)
    {
        auto var = FunctionFactory::Create(&A);
        std::vector<FunctionType> allowedFunctions{};
        std::vector<double*> allowedTerminals{ &B };

        // since there is only one node (Variable), and only one allowed terminal,
        // the mutation should change this to B.
        Operators::Mutate(var, allowedFunctions, allowedTerminals);
        ASSERT_DOUBLE_EQ(3.3, var->Evaluate());
    }

    TEST(OperatorsTest, MutateEitherVariableOrFunction)
    {
        double two = 2.0;
        double four = 4.0;
        auto func = FunctionFactory::Create(FunctionType::Addition);
        func->AddChild(FunctionFactory::Create(&four));
        std::vector<FunctionType> allowedFunctions{ FunctionType::SquareRoot };
        std::vector<double*> allowedTerminals{ &two };
        Operators::Mutate(func, allowedFunctions, allowedTerminals);

        ASSERT_DOUBLE_EQ(2.0, func->Evaluate());
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
}