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
                const std::string& symbol,
                int minChildren = 1,
                int maxChildren = std::numeric_limits<int>::max());

        Function(const Function& other);

        /**
         * @see INode::Evaluate
         */
        double Evaluate() const override;

        /**
         * @see INode::ToString
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
         * @see INode::GetChildren
         */
        const std::vector<std::unique_ptr<INode>>& GetChildren() const override;

        /**
         * @see INode::MaxChildren()
         */
        int MaxChildren() const override;

        /**
         * @see INode::Size()
         */
        int Size() const override;

        /**
         * @see INode::Get
         */
        std::unique_ptr<INode>& Get(int index, std::unique_ptr<INode>& ptr) override;

        /**
         * @see INode::Clone
         */
        std::unique_ptr<INode> Clone() const override;

        /**
         * returns true if the number of children is less than the minimum required
         */
        bool LacksBreadth() const override;

    private:
        /**
         * @see INode::GetSymbol
         */
        std::string GetSymbol() const override;

        ChildNodes m_children;
        const int MinAllowedChildren;
        const int MaxAllowedChildren;
        const std::function<double(const ChildNodes&)> m_func;
        const std::string m_symbol; ///< A symbolic representation of the function
    };
}
#endif
