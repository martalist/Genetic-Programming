#ifndef FunctionFactory_H
#define FunctionFactory_H

#include <memory>
#include <string>
#include "INode.h"

namespace Model
{
    enum class FunctionType
    {
        None = 0,
        Addition,
        Subtraction,
        Multiplication,
        Division,
        SquareRoot,
        Sine,
        Cosine,
        NaturalExponential,
        NaturalLogarithm
    };

    /**
     * An interface Node of the genetic programming tree/model.
     */
    class FunctionFactory
    {
    public:
        /**
         * Create a function
         * @param type The type of function to create
         * @return the new INode
         */
        static std::unique_ptr<INode> Create(const FunctionType& type);

        /**
         * Create a variable
         * @param varaible A pointer to the variable
         * @return the new INode
         */
        static std::unique_ptr<INode> Create(const double* variable);

        /**
         * @param type The enum represtionation of a function type
         * @return The string representation of a function type
         */
        static std::string AsString(const FunctionType& type);

        /**
         * @param name The string representation of a function type
         * @return The enum represtionation of a function type
         */
        static FunctionType AsFunctionType(const std::string& name);

    private:
        /**
         * Create an addition function
         */
        static std::unique_ptr<INode> CreateAddition();

        /**
         * Create a subtraction function
         */
        static std::unique_ptr<INode> CreateSubtraction();

        /**
         * Create a multiplication function
         */
        static std::unique_ptr<INode> CreateMultiplication();

        /**
         * Create a division function
         */
        static std::unique_ptr<INode> CreateDivision();

        /**
         * Create a square root function
         */
        static std::unique_ptr<INode> CreateSquareRoot();

        /**
         * Create a sine function
         */
        static std::unique_ptr<INode> CreateSine();

        /**
         * Create a cosine function
         */
        static std::unique_ptr<INode> CreateCosine();

        /**
         * Create a natural exponential function
         */
        static std::unique_ptr<INode> CreateExponential();

        /**
         * Create a natural (base e) logarithm function
         */
        static std::unique_ptr<INode> CreateLog();
    };
}
#endif
