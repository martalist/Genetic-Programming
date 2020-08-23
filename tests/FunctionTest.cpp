#include <gtest/gtest.h>
#include <memory>
#include "../src/model/Variable.h"
#include "../src/model/Function.h"
#include "../src/model/FunctionFactory.h"

namespace Tests
{
    using namespace Model;

    class FunctionTest : public ::testing::Test
    {
    protected:
        FunctionTest() { }
        ~FunctionTest() = default;

        const double a = 1.0;
        const double b = 2.0;
        const double c = 3.0;
    };


    TEST_F(FunctionTest, Addition)
    {
        auto func = FunctionFactory::Create(FunctionType::Addition);
        func->AddChild(std::make_unique<Variable>(a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(b));
        ASSERT_DOUBLE_EQ(3.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(c));
        ASSERT_DOUBLE_EQ(6.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Subtraction)
    {
        auto func = FunctionFactory::Create(FunctionType::Subtraction);
        func->AddChild(std::make_unique<Variable>(a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(b));
        ASSERT_DOUBLE_EQ(-1.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(c));
        ASSERT_DOUBLE_EQ(-4.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Multiplication)
    {
        auto func = FunctionFactory::Create(FunctionType::Multiplication);
        func->AddChild(std::make_unique<Variable>(a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(b));
        ASSERT_DOUBLE_EQ(2.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(c));
        ASSERT_DOUBLE_EQ(6.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Division)
    {
        auto func = FunctionFactory::Create(FunctionType::Division);
        func->AddChild(std::make_unique<Variable>(a));
        // ASSERT_DOUBLE_EQ(1.0, func->Evaluate()); // TODO: what happens if we only have a numerator
        // but no denominator? .... should this be allowed?

        func->AddChild(std::make_unique<Variable>(b));
        ASSERT_DOUBLE_EQ(0.5, func->Evaluate());

        func = FunctionFactory::Create(FunctionType::Division);
        func->AddChild(std::make_unique<Variable>(c));
        func->AddChild(std::make_unique<Variable>(a));
        ASSERT_DOUBLE_EQ(3.0, func->Evaluate());

        func = FunctionFactory::Create(FunctionType::Division);
        func->AddChild(std::make_unique<Variable>(c));
        func->AddChild(std::make_unique<Variable>(b));
        ASSERT_DOUBLE_EQ(1.5, func->Evaluate());
    }
}
