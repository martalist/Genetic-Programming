#ifndef FunctionFactory_H
#define FunctionFactory_H

#include <memory>

namespace Model
{
    class INode;

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
         */
        static std::unique_ptr<INode> Create(const FunctionType& type);

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
