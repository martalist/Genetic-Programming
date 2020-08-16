#ifndef Variable_H
#define Variable_H

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
    class Variable : public INode
    {
    public:
        /**
         * Constructor
         * @param variable A reference to the variable.
         */
        Variable(const double& variable);

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

        /**
         * @see INode::SwapWith
         */
        bool SwapWith(std::unique_ptr<INode> child) override;

        bool AddChild(std::unique_ptr<INode> child) override;

        int NumberOfChildren() const override;

    private:
        const double& m_variable;
    };
}
#endif
