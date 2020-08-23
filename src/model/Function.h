#ifndef Function_H
#define Function_H

#include <functional>
#include <limits>
#include <memory>
#include <vector>

#include "INode.h"

namespace Model
{
    typedef std::vector<std::unique_ptr<INode>> ChildNodes;

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
                int maxChildren = std::numeric_limits<int>::max());

        Function(const Function& other);

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
         * @see INode::MoveChildrenTo
         */
        bool MoveChildrenTo(std::unique_ptr<INode>& other) override;

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

        ChildNodes m_children;
        const int MaxAllowedChildren;
        const std::function<double(const ChildNodes&)> m_func;
    };
}
#endif
