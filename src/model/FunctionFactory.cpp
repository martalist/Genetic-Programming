#include "FunctionFactory.h"

#include <numeric>
#include "Function.h"

namespace Model
{
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
        auto func = [](const ChildNodes& children) 
        {
            return children[0]->Evaluate() / children[1]->Evaluate();
        };
        return std::make_unique<Function>(func, 2);
    }
}
