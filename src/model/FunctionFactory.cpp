#include "FunctionFactory.h"

#include <cmath>
#include <numeric>
#include "Function.h"
#include "Variable.h"

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
            throw std::exception();
        }
    }
    std::unique_ptr<INode> FunctionFactory::Create(const double* variable)
    {
        return std::make_unique<Variable>(variable);
    }

    std::unique_ptr<INode> FunctionFactory::Copy(const INode& other)
    {
        // TODO: This implementation has a bad smell to it
        if (other.IsVariable())
        {
            return std::make_unique<Variable>(dynamic_cast<const Variable&>(other));
        }
        return std::make_unique<Function>(dynamic_cast<const Function&>(other));
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
        return std::make_unique<Function>(func);
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
        return std::make_unique<Function>(func);
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
        return std::make_unique<Function>(func);
    }

    std::unique_ptr<INode> FunctionFactory::CreateDivision()
    {
        auto func = [](const ChildNodes& children) -> double
        {
            if (children.size() != 2u)
            {
                throw std::exception();
            }
            return children[0]->Evaluate() / children[1]->Evaluate();
        };
        return std::make_unique<Function>(func, 2);
    }

    std::unique_ptr<INode> FunctionFactory::CreateSquareRoot()
    {
        auto func = [](const ChildNodes& children) 
        {
            if (children.size() != 1u)
            {
                throw std::exception();
            }
            return std::sqrt(children[0]->Evaluate());
        };
        return std::make_unique<Function>(func, 1);
    }
}
