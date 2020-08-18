#ifndef FunctionFactory_H
#define FunctionFactory_H

#include <memory>
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
        SquareRoot
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
         */
        static std::unique_ptr<INode> Create(const FunctionType& type);

        /**
         * Performs a deep copy of the node tree
         * @param other The node tree to copy
         */
        static std::unique_ptr<INode> Copy(const INode& other);

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
    };
}
#endif
