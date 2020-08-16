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
}
