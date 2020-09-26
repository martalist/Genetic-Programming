#ifndef Terminal_H
#define Terminal_H

#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <vector>

#include "INode.h"

namespace Model
{
    /**
     * An interface Node of the genetic programming tree/model.
     */
    class Terminal : public INode
    {
    public:
        /**
         * Constructor
         * @param variable A reference to the variable.
         */
        Terminal(const double* variable);

        Terminal(const Terminal& other);

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
         * @see INode::Get
         */
        std::unique_ptr<INode>& Get(int index, std::unique_ptr<INode>& ptr) override;

        /**
         * @see INode::Clone
         */
        std::unique_ptr<INode> Clone() const override;

    private:
        const double* m_variable; ///< A pointer to the terminal value
        std::string m_symbol; ///< A symbolic representation of the terminal

        static char letter; ///< The symbol for the next terminal
        static std::map<const double*, char> s_symbols; ///< Maps symbols to the terminal pointer
    };
}
#endif
