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
        func->AddChild(std::make_unique<Variable>(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(&b));
        ASSERT_DOUBLE_EQ(3.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(&c));
        ASSERT_DOUBLE_EQ(6.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Subtraction)
    {
        auto func = FunctionFactory::Create(FunctionType::Subtraction);
        func->AddChild(std::make_unique<Variable>(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(&b));
        ASSERT_DOUBLE_EQ(-1.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(&c));
        ASSERT_DOUBLE_EQ(-4.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Multiplication)
    {
        auto func = FunctionFactory::Create(FunctionType::Multiplication);
        func->AddChild(std::make_unique<Variable>(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(&b));
        ASSERT_DOUBLE_EQ(2.0, func->Evaluate());

        func->AddChild(std::make_unique<Variable>(&c));
        ASSERT_DOUBLE_EQ(6.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Division)
    {
        auto func = FunctionFactory::Create(FunctionType::Division);
        func->AddChild(std::make_unique<Variable>(&a));
        // ASSERT_DOUBLE_EQ(1.0, func->Evaluate()); // TODO: what happens if we only have a numerator
        // but no denominator? .... should this be allowed?

        func->AddChild(std::make_unique<Variable>(&b));
        ASSERT_DOUBLE_EQ(0.5, func->Evaluate());

        func = FunctionFactory::Create(FunctionType::Division);
        func->AddChild(std::make_unique<Variable>(&c));
        func->AddChild(std::make_unique<Variable>(&a));
        ASSERT_DOUBLE_EQ(3.0, func->Evaluate());

        func = FunctionFactory::Create(FunctionType::Division);
        ASSERT_TRUE(func->AddChild(std::make_unique<Variable>(&c)));
        ASSERT_TRUE(func->AddChild(std::make_unique<Variable>(&b)));
        ASSERT_DOUBLE_EQ(1.5, func->Evaluate());

        // division can only have 2 children
        ASSERT_FALSE(func->AddChild(std::make_unique<Variable>(&b)));
    }

    TEST_F(FunctionTest, SquareRoot)
    {
        auto func = FunctionFactory::Create(FunctionType::SquareRoot);
        func->AddChild(std::make_unique<Variable>(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        const double four = 4.0;
        func = FunctionFactory::Create(FunctionType::SquareRoot);
        ASSERT_TRUE(func->AddChild(std::make_unique<Variable>(&four)));
        ASSERT_DOUBLE_EQ(2.0, func->Evaluate());

        // sqrt can only have one child
        ASSERT_FALSE(func->AddChild(std::make_unique<Variable>(&b)));
    }

    TEST_F(FunctionTest, CompositeFunction)
    {

        auto root = FunctionFactory::Create(FunctionType::SquareRoot);
        auto div = FunctionFactory::Create(FunctionType::Division);
        auto mult = FunctionFactory::Create(FunctionType::Multiplication);
        auto add = FunctionFactory::Create(FunctionType::Addition);
        auto sub = FunctionFactory::Create(FunctionType::Subtraction);

        // (sqrt (/ (* b (+ a b c)) (- c b))) = sqrt(12)
        add->AddChild(std::make_unique<Variable>(&a));
        add->AddChild(std::make_unique<Variable>(&b));
        add->AddChild(std::make_unique<Variable>(&c));
        sub->AddChild(std::make_unique<Variable>(&c));
        sub->AddChild(std::make_unique<Variable>(&b));
        mult->AddChild(std::make_unique<Variable>(&b));
        mult->AddChild(std::move(add));
        div->AddChild(std::move(mult));
        div->AddChild(std::move(sub));
        root->AddChild(std::move(div));

        ASSERT_DOUBLE_EQ(3.4641016151377544, root->Evaluate());
    }
}
