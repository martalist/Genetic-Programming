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

        Variable(const Variable& other);

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
        bool SwapWith(std::unique_ptr<INode> other) override;

        /**
         * @see INode::AddChild()
         */
        bool AddChild(std::unique_ptr<INode> child) override;

        /**
         * @see INode::NumberOfChildren()
         */
        int NumberOfChildren() const override;

        /**
         * @see INode::MaxChildren()
         */
        int MaxChildren() const override;

        /**
         * @see INode::Size()
         */
        int Size() const override;

        /**
         * @see INode::IsVariable()
         */
        bool IsVariable() const override;

        /**
         * @see INode::Get
         */
        std::unique_ptr<INode>& Get(int index) override;

    private:
        const double& m_variable;
    };
}
#endif
