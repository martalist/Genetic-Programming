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
        std::vector<FunctionType> allowed{ FunctionType::Subtraction };
        std::vector<double*> terminals{ &A, &B };

        // since there is only one node, and only one allowed function,
        // the mutation should change this to a sutraction
        Operators::Mutate(func, allowed, terminals);
        func->AddChild(std::make_unique<Variable>(B));
        func->AddChild(std::make_unique<Variable>(A));
        ASSERT_DOUBLE_EQ(1.8, func->Evaluate());
    }
}
