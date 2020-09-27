#include "FunctionFactory.h"

#include <cmath>
#include <numeric>
#include <stdexcept>
#include "Function.h"
#include "Terminal.h"

namespace Model
{
    std::unique_ptr<INode> FunctionFactory::Create(const FunctionType& type)
    {
        switch (type)
        {
        case FunctionType::Addition:
            return CreateAddition();
        case FunctionType::Subtraction:
            return CreateSubtraction();
        case FunctionType::Multiplication:
            return CreateMultiplication();
        case FunctionType::Division:
            return CreateDivision();
        case FunctionType::SquareRoot:
            return CreateSquareRoot();
        default:
            throw std::invalid_argument("The function type provided is not valid");
        }
    }

    std::unique_ptr<INode> FunctionFactory::Create(const double* variable)
    {
        return std::make_unique<Terminal>(variable);
    }

    std::string FunctionFactory::AsString(const FunctionType& type)
    {
        switch (type)
        {
        case FunctionType::Addition:
            return "Addition";
        case FunctionType::Subtraction:
            return "Subtraction";
        case FunctionType::Multiplication:
            return "Multiplication";
        case FunctionType::Division:
            return "Division";
        case FunctionType::SquareRoot:
            return "Square Root";
        default:
            throw std::invalid_argument("The function type provided is not valid");
        }
    }

    std::unique_ptr<INode> FunctionFactory::CreateAddition()
    {
        auto func = [](const ChildNodes& children) -> double
        {
            double result = 0.0;
            if (!children.empty())
            {
                for (auto& child : children)
                {
                    result += child->Evaluate();
                }
            }
            return result;
        };
        return std::make_unique<Function>(func, "+", 2);
    }

    std::unique_ptr<INode> FunctionFactory::CreateSubtraction()
    {
        auto func = [](const ChildNodes& children) -> double
        {
            double result = children.empty() ? 0.0 : children[0]->Evaluate();
            for (auto i = 1u; i < children.size(); ++i)
            {
                result -= children[i]->Evaluate();
            }
            return result;
        };
        return std::make_unique<Function>(func, "-", 2);
    }


    std::unique_ptr<INode> FunctionFactory::CreateMultiplication()
    {
        auto func = [](const ChildNodes& children) 
        {
            double result = 1.0;
            for (auto& child : children)
            {
                result *= child->Evaluate();
            }
            return result;
        };
        return std::make_unique<Function>(func, "*", 2);
    }

    std::unique_ptr<INode> FunctionFactory::CreateDivision()
    {
        auto func = [](const ChildNodes& children) -> double
        {
            if (children.size() == 1)
            {
                // TODO: consider having random creationg check for a MinChildren instead
                return children[0]->Evaluate(); // assumes that the demoninator is 1
            }
            if (children.size() == 2)
            {
                return children[0]->Evaluate() / children[1]->Evaluate();
            }
            throw std::logic_error("A division function must have no more than 2 children.");
        };
        return std::make_unique<Function>(func, "/", 2, 2);
    }

    std::unique_ptr<INode> FunctionFactory::CreateSquareRoot()
    {
        auto func = [](const ChildNodes& children) -> double
        {
            if (children.size() != 1u)
            {
                throw std::logic_error("A square root function must have exactly 1 child.");
            }

            auto mantissa = children[0]->Evaluate();
            if (mantissa >= 0)
            {
                return std::sqrt(mantissa);
            }
            return NAN; // we're only dealing with real numbers here
        };
        return std::make_unique<Function>(func, "√", 1, 1);
    }
}
