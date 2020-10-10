#include <gtest/gtest.h>
#include <memory>
#include "../src/model/Terminal.h"
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
        func->AddChild(std::make_unique<Terminal>(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(std::make_unique<Terminal>(&b));
        ASSERT_DOUBLE_EQ(3.0, func->Evaluate());

        func->AddChild(std::make_unique<Terminal>(&c));
        ASSERT_DOUBLE_EQ(6.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Subtraction)
    {
        auto func = FunctionFactory::Create(FunctionType::Subtraction);
        func->AddChild(std::make_unique<Terminal>(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(std::make_unique<Terminal>(&b));
        ASSERT_DOUBLE_EQ(-1.0, func->Evaluate());

        func->AddChild(std::make_unique<Terminal>(&c));
        ASSERT_DOUBLE_EQ(-4.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Multiplication)
    {
        auto func = FunctionFactory::Create(FunctionType::Multiplication);
        ASSERT_EQ(1, func->Size());
        func->AddChild(std::make_unique<Terminal>(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());
        ASSERT_EQ(2, func->Size());

        func->AddChild(std::make_unique<Terminal>(&b));
        ASSERT_DOUBLE_EQ(2.0, func->Evaluate());
        ASSERT_EQ(3, func->Size());

        func->AddChild(std::make_unique<Terminal>(&c));
        ASSERT_DOUBLE_EQ(6.0, func->Evaluate());
        ASSERT_EQ(4, func->Size());
    }

    TEST_F(FunctionTest, Division)
    {
        auto func = FunctionFactory::Create(FunctionType::Division);
        func->AddChild(std::make_unique<Terminal>(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());
        // TODO: no denominator .... should this be allowed?

        func->AddChild(std::make_unique<Terminal>(&b));
        ASSERT_DOUBLE_EQ(0.5, func->Evaluate());

        func = FunctionFactory::Create(FunctionType::Division);
        func->AddChild(std::make_unique<Terminal>(&c));
        func->AddChild(std::make_unique<Terminal>(&a));
        ASSERT_DOUBLE_EQ(3.0, func->Evaluate());

        func = FunctionFactory::Create(FunctionType::Division);
        ASSERT_TRUE(func->AddChild(std::make_unique<Terminal>(&c)));
        ASSERT_TRUE(func->AddChild(std::make_unique<Terminal>(&b)));
        ASSERT_DOUBLE_EQ(1.5, func->Evaluate());

        // division can only have 2 children
        ASSERT_FALSE(func->AddChild(std::make_unique<Terminal>(&b)));
    }

    TEST_F(FunctionTest, SquareRoot)
    {
        auto func = FunctionFactory::Create(FunctionType::SquareRoot);
        func->AddChild(std::make_unique<Terminal>(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        const double four = 4.0;
        func = FunctionFactory::Create(FunctionType::SquareRoot);
        ASSERT_TRUE(func->AddChild(std::make_unique<Terminal>(&four)));
        ASSERT_DOUBLE_EQ(2.0, func->Evaluate());
        ASSERT_EQ(2, func->Size());

        // sqrt can only have one child
        ASSERT_FALSE(func->AddChild(std::make_unique<Terminal>(&b)));

        // we're only dealing with real numbers, sqrt of a negative number should be NAN
        const double minus = -1.0;
        func = FunctionFactory::Create(FunctionType::SquareRoot);
        ASSERT_TRUE(func->AddChild(std::make_unique<Terminal>(&minus)));
        ASSERT_TRUE(std::isnan(func->Evaluate()));
    }

    TEST_F(FunctionTest, CompositeFunction)
    {

        auto root = FunctionFactory::Create(FunctionType::SquareRoot);
        auto div = FunctionFactory::Create(FunctionType::Division);
        auto mult = FunctionFactory::Create(FunctionType::Multiplication);
        auto add = FunctionFactory::Create(FunctionType::Addition);
        auto sub = FunctionFactory::Create(FunctionType::Subtraction);

        // (sqrt (/ (* b (+ a b c)) (- c b))) = sqrt(12)
        add->AddChild(std::make_unique<Terminal>(&a));
        add->AddChild(std::make_unique<Terminal>(&b));
        add->AddChild(std::make_unique<Terminal>(&c));
        sub->AddChild(std::make_unique<Terminal>(&c));
        sub->AddChild(std::make_unique<Terminal>(&b));
        mult->AddChild(std::make_unique<Terminal>(&b));
        mult->AddChild(std::move(add));
        div->AddChild(std::move(mult));
        div->AddChild(std::move(sub));
        root->AddChild(std::move(div));

        ASSERT_DOUBLE_EQ(3.4641016151377544, root->Evaluate());
    }

    TEST_F(FunctionTest, GetNodePointers)
    {
        auto root = FunctionFactory::Create(FunctionType::SquareRoot);
        auto div = FunctionFactory::Create(FunctionType::Division);
        auto mult = FunctionFactory::Create(FunctionType::Multiplication);
        auto add = FunctionFactory::Create(FunctionType::Addition);
        auto sub = FunctionFactory::Create(FunctionType::Subtraction);
        add->AddChild(FunctionFactory::Create(&a));
        add->AddChild(FunctionFactory::Create(&b));
        add->AddChild(FunctionFactory::Create(&c));
        sub->AddChild(FunctionFactory::Create(&c));
        sub->AddChild(FunctionFactory::Create(&b));
        mult->AddChild(FunctionFactory::Create(&b));
        mult->AddChild(std::move(add));
        div->AddChild(std::move(mult));
        div->AddChild(std::move(sub));
        root->AddChild(std::move(div));

        /*               sqrt
         *                |
         *               div
         *              /   \
         *          mult     minus
         *         /   \     /   \
         *        b   plus  c     b
         *           / | \
         *          a  b  c
         *
         *   a = 1.0;  b = 2.0;  c = 3.0;
         */

        ASSERT_EQ(11, root->Size());
        ASSERT_DOUBLE_EQ(12.0, root->Get(1, root)->Evaluate()); // div
        ASSERT_DOUBLE_EQ(12.0, root->Get(2, root)->Evaluate()); // mult
        ASSERT_DOUBLE_EQ(2.0, root->Get(3, root)->Evaluate()); // b
        ASSERT_DOUBLE_EQ(6.0, root->Get(4, root)->Evaluate()); // plus
        ASSERT_DOUBLE_EQ(1.0, root->Get(5, root)->Evaluate()); // a
        ASSERT_DOUBLE_EQ(2.0, root->Get(6, root)->Evaluate()); // b
        ASSERT_DOUBLE_EQ(3.0, root->Get(7, root)->Evaluate()); // c
        ASSERT_DOUBLE_EQ(1.0, root->Get(8, root)->Evaluate()); // minus
        ASSERT_DOUBLE_EQ(3.0, root->Get(9, root)->Evaluate()); // c
        ASSERT_DOUBLE_EQ(2.0, root->Get(10, root)->Evaluate()); // b
    }

    TEST_F(FunctionTest, CloneSingleFunctionAndChild)
    {
        auto root = FunctionFactory::Create(FunctionType::Addition);
        root->AddChild(FunctionFactory::Create(&a));

        auto clone = root->Clone();
        ASSERT_DOUBLE_EQ(root->Evaluate(), clone->Evaluate());
        ASSERT_EQ(root->ToString(), clone->ToString());
    }

    TEST_F(FunctionTest, CloneSExpression)
    {
        auto root = FunctionFactory::Create(FunctionType::SquareRoot);
        auto div = FunctionFactory::Create(FunctionType::Division);
        auto mult = FunctionFactory::Create(FunctionType::Multiplication);
        auto add = FunctionFactory::Create(FunctionType::Addition);
        auto sub = FunctionFactory::Create(FunctionType::Subtraction);
        add->AddChild(FunctionFactory::Create(&a));
        add->AddChild(FunctionFactory::Create(&b));
        add->AddChild(FunctionFactory::Create(&c));
        sub->AddChild(FunctionFactory::Create(&c));
        sub->AddChild(FunctionFactory::Create(&b));
        mult->AddChild(FunctionFactory::Create(&b));
        mult->AddChild(std::move(add));
        div->AddChild(std::move(mult));
        div->AddChild(std::move(sub));
        root->AddChild(std::move(div));

        auto clone = root->Clone();
        ASSERT_DOUBLE_EQ(root->Evaluate(), clone->Evaluate());
        ASSERT_EQ(root->ToString(), clone->ToString());
    }
}
