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
        FunctionFactory factory;
    };


    TEST_F(FunctionTest, Addition)
    {
        auto func = factory.Create(FunctionType::Addition);
        func->AddChild(factory.Create(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(factory.Create(&b));
        ASSERT_DOUBLE_EQ(3.0, func->Evaluate());

        func->AddChild(factory.Create(&c));
        ASSERT_DOUBLE_EQ(6.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Subtraction)
    {
        auto func = factory.Create(FunctionType::Subtraction);
        func->AddChild(factory.Create(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        func->AddChild(factory.Create(&b));
        ASSERT_DOUBLE_EQ(-1.0, func->Evaluate());

        func->AddChild(factory.Create(&c));
        ASSERT_DOUBLE_EQ(-4.0, func->Evaluate());
    }

    TEST_F(FunctionTest, Multiplication)
    {
        auto func = factory.Create(FunctionType::Multiplication);
        ASSERT_EQ(1, func->Size());
        func->AddChild(factory.Create(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());
        ASSERT_EQ(2, func->Size());

        func->AddChild(factory.Create(&b));
        ASSERT_DOUBLE_EQ(2.0, func->Evaluate());
        ASSERT_EQ(3, func->Size());

        func->AddChild(factory.Create(&c));
        ASSERT_DOUBLE_EQ(6.0, func->Evaluate());
        ASSERT_EQ(4, func->Size());
    }

    TEST_F(FunctionTest, Division)
    {
        auto func = factory.Create(FunctionType::Division);
        func->AddChild(factory.Create(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());
        // TODO: no denominator .... should this be allowed?

        func->AddChild(factory.Create(&b));
        ASSERT_DOUBLE_EQ(0.5, func->Evaluate());

        func = factory.Create(FunctionType::Division);
        func->AddChild(factory.Create(&c));
        func->AddChild(factory.Create(&a));
        ASSERT_DOUBLE_EQ(3.0, func->Evaluate());

        func = factory.Create(FunctionType::Division);
        ASSERT_TRUE(func->AddChild(factory.Create(&c)));
        ASSERT_TRUE(func->AddChild(factory.Create(&b)));
        ASSERT_DOUBLE_EQ(1.5, func->Evaluate());

        // division can only have 2 children
        ASSERT_FALSE(func->AddChild(factory.Create(&b)));
    }

    TEST_F(FunctionTest, SquareRoot)
    {
        auto func = factory.Create(FunctionType::SquareRoot);
        func->AddChild(factory.Create(&a));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());

        const double four = 4.0;
        func = factory.Create(FunctionType::SquareRoot);
        ASSERT_TRUE(func->AddChild(factory.Create(&four)));
        ASSERT_DOUBLE_EQ(2.0, func->Evaluate());
        ASSERT_EQ(2, func->Size());

        // sqrt can only have one child
        ASSERT_FALSE(func->AddChild(factory.Create(&b)));

        // we're only dealing with real numbers, so sqrt should be of abs value
        const double minus = -1.0;
        func = factory.Create(FunctionType::SquareRoot);
        ASSERT_TRUE(func->AddChild(factory.Create(&minus)));
        ASSERT_DOUBLE_EQ(1.0, func->Evaluate());
    }

    TEST_F(FunctionTest, CompositeFunction)
    {

        auto root = factory.Create(FunctionType::SquareRoot);
        auto div = factory.Create(FunctionType::Division);
        auto mult = factory.Create(FunctionType::Multiplication);
        auto add = factory.Create(FunctionType::Addition);
        auto sub = factory.Create(FunctionType::Subtraction);

        // (sqrt (/ (* b (+ a b c)) (- c b))) = sqrt(12)
        add->AddChild(factory.Create(&a));
        add->AddChild(factory.Create(&b));
        add->AddChild(factory.Create(&c));
        sub->AddChild(factory.Create(&c));
        sub->AddChild(factory.Create(&b));
        mult->AddChild(factory.Create(&b));
        mult->AddChild(std::move(add));
        div->AddChild(std::move(mult));
        div->AddChild(std::move(sub));
        root->AddChild(std::move(div));

        ASSERT_DOUBLE_EQ(3.4641016151377544, root->Evaluate());
    }

    TEST_F(FunctionTest, GetNodePointers)
    {
        auto root = factory.Create(FunctionType::SquareRoot);
        auto div = factory.Create(FunctionType::Division);
        auto mult = factory.Create(FunctionType::Multiplication);
        auto add = factory.Create(FunctionType::Addition);
        auto sub = factory.Create(FunctionType::Subtraction);
        add->AddChild(factory.Create(&a));
        add->AddChild(factory.Create(&b));
        add->AddChild(factory.Create(&c));
        sub->AddChild(factory.Create(&c));
        sub->AddChild(factory.Create(&b));
        mult->AddChild(factory.Create(&b));
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
        auto root = factory.Create(FunctionType::Addition);
        root->AddChild(factory.Create(&a));

        auto clone = root->Clone();
        ASSERT_DOUBLE_EQ(root->Evaluate(), clone->Evaluate());
        ASSERT_EQ(root->ToString(), clone->ToString());
    }

    TEST_F(FunctionTest, CloneSExpression)
    {
        auto root = factory.Create(FunctionType::SquareRoot);
        auto div = factory.Create(FunctionType::Division);
        auto mult = factory.Create(FunctionType::Multiplication);
        auto add = factory.Create(FunctionType::Addition);
        auto sub = factory.Create(FunctionType::Subtraction);
        add->AddChild(factory.Create(&a));
        add->AddChild(factory.Create(&b));
        add->AddChild(factory.Create(&c));
        sub->AddChild(factory.Create(&c));
        sub->AddChild(factory.Create(&b));
        mult->AddChild(factory.Create(&b));
        mult->AddChild(std::move(add));
        div->AddChild(std::move(mult));
        div->AddChild(std::move(sub));
        root->AddChild(std::move(div));

        auto clone = root->Clone();
        ASSERT_DOUBLE_EQ(root->Evaluate(), clone->Evaluate());
        ASSERT_EQ(root->ToString(), clone->ToString());
    }
}
