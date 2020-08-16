#include "FunctionFactory.h"

#include <cmath>
#include <numeric>
#include "Function.h"

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

    std::unique_ptr<INode> FunctionFactory::CreateAddition()
    {
        auto func = [](const ChildNodes& children) 
        {
            return std::accumulate(children.begin(), children.end(), 0.0,
                [](double b, const std::unique_ptr<Model::INode>& child) 
                { 
                    return b + child->Evaluate(); 
                });
        };
        return std::make_unique<Function>(func);
    }

    std::unique_ptr<INode> FunctionFactory::CreateSubtraction()
    {
        auto func = [](const ChildNodes& children)
        {
            return std::accumulate(children.begin() + 1, 
                    children.end(), 
                    (*children.begin())->Evaluate(),
                [](double b, const std::unique_ptr<Model::INode>& child) 
                { 
                    return b - child->Evaluate(); 
                });
        };
        return std::make_unique<Function>(func);
    }


    std::unique_ptr<INode> FunctionFactory::CreateMultiplication()
    {
        auto func = [](const ChildNodes& children) 
        {
            return std::accumulate(children.begin() + 1, 
                    children.end(), 
                    (*children.begin())->Evaluate(),
                [](double b, const std::unique_ptr<Model::INode>& child) 
                { 
                    return b * child->Evaluate(); 
                });
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
