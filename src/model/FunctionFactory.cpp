#include "FunctionFactory.h"

#include <cmath>
#include <numeric>
#include <stdexcept>
#include "Function.h"
#include "Terminal.h"

namespace
{
    const std::string Add = "Addition";
    const std::string Sub = "Subtraction";
    const std::string Mult = "Multiplication";
    const std::string Div = "Division";
    const std::string Sqrt = "Square Root";
    const std::string Sin = "Sine";
    const std::string Cos = "Cosine";
    const std::string Exp = "Exponential";
    const std::string Log = "Logarithm";

    const double Threshold = 0.001; // A threshold used to prevent infty for division, sqrt, log, etc.
}

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
        case FunctionType::Sine:
            return CreateSine();
        case FunctionType::Cosine:
            return CreateCosine();
        case FunctionType::NaturalExponential:
            return CreateExponential();
        case FunctionType::NaturalLogarithm:
            return CreateLog();
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
            return Add;
        case FunctionType::Subtraction:
            return Sub;
        case FunctionType::Multiplication:
            return Mult;
        case FunctionType::Division:
            return Div;
        case FunctionType::SquareRoot:
            return Sqrt;
        case FunctionType::Sine:
            return Sin;
        case FunctionType::Cosine:
            return Cos;
        case FunctionType::NaturalExponential:
            return Exp;
        case FunctionType::NaturalLogarithm:
            return Log;
        default:
            throw std::invalid_argument("The function type provided is not valid");
        }
    }

    FunctionType FunctionFactory::AsFunctionType(const std::string& name)
    {
        if (name == Add)
        {
            return FunctionType::Addition;
        }
        if (name == Sub)
        {
            return FunctionType::Subtraction;
        }
        if (name == Mult)
        {
            return FunctionType::Multiplication;
        }
        if (name == Div)
        {
            return FunctionType::Division;
        }
        if (name == Sqrt)
        {
            return FunctionType::SquareRoot;
        }
        if (name == Sin)
        {
            return FunctionType::Sine;
        }
        if (name == Cos)
        {
            return FunctionType::Cosine;
        }
        if (name == Exp)
        {
            return FunctionType::NaturalExponential;
        }
        if (name == Log)
        {
            return FunctionType::NaturalLogarithm;
        }
        throw std::invalid_argument("The name provided does not specify a valid function type");
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
                auto denom = children[1]->Evaluate();
                if (std::abs(denom) < Threshold)
                { // 
                    return 1.0;
                }
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
            if (children.size() != 1)
            {
                throw std::logic_error("A square root function must have exactly 1 child.");
            }

            // returns sqrt(|x|) to prevent NaN
            return std::sqrt(std::abs(children[0]->Evaluate()));
        };
        return std::make_unique<Function>(func, "√", 1, 1);
    }

    std::unique_ptr<INode> FunctionFactory::CreateSine()
    {
        auto func = [](const ChildNodes& children) -> double
        {
            if (children.size() != 1)
            {
                throw std::logic_error("A sine function must have exactly 1 child.");
            }
            return std::sin(children[0]->Evaluate());
        };
        return std::make_unique<Function>(func, "sin", 1, 1);
    }

    std::unique_ptr<INode> FunctionFactory::CreateCosine()
    {
        auto func = [](const ChildNodes& children) -> double
        {
            if (children.size() != 1)
            {
                throw std::logic_error("A cosine function must have exactly 1 child.");
            }
            return std::cos(children[0]->Evaluate());
        };
        return std::make_unique<Function>(func, "cos", 1, 1);
    }

    std::unique_ptr<INode> FunctionFactory::CreateExponential()
    {
        auto func = [](const ChildNodes& children) -> double
        {
            if (children.size() != 1)
            {
                throw std::logic_error("A cosine function must have exactly 1 child.");
            }
            return std::exp(children[0]->Evaluate());
        };
        return std::make_unique<Function>(func, "e^", 1, 1);
    }

    std::unique_ptr<INode> FunctionFactory::CreateLog()
    {
        auto func = [](const ChildNodes& children) -> double
        {
            if (children.size() != 1)
            {
                throw std::logic_error("A logarithm function must have exactly 1 child.");
            }

            auto child = std::abs(children[0]->Evaluate());
            if (child < Threshold)
            {
                return 0.0;
            }
            return std::log(child);
        };
        return std::make_unique<Function>(func, "ln", 1, 1);
    }
}
