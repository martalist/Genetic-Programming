#ifndef Function_H
#define Function_H

#include <functional>
#include <limits>
#include <memory>
#include <vector>

#include "INode.h"

namespace Model
{
    /**
     * An interface Node of the genetic programming tree/model.
     */
    class Function : public INode
    {
    public:
        /**
         * Constructor
         * @param func The mathematical function to call upon the child nodes
         * @param maxChildren The max legal number of children for the function
         */
        Function(std::function<double(const ChildNodes&)> func, 
                int maxChildren = std::numeric_limits<int>::infinity());

        /**
         * Evaluates the value of this subtree.
         * @return the primitate value for a variable, or the return 
         * value for a function
         */
        double Evaluate() const override;

        /**
         * @return the string representation of this subtree.
         */
        std::string ToString() const override;

        bool AddChild(std::unique_ptr<INode> child);

        bool SwapWith(std::unique_ptr<INode> child);

    private:
        ChildNodes m_children;
        const int MaxChildren;
        const std::function<double(const ChildNodes&)> m_func;
    };
}
#endif
