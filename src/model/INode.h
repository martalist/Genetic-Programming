#ifndef INode_H
#define INode_H

#include <memory>
#include <string>
#include <vector>

namespace Model
{
    /**
     * An interface Node of the genetic programming tree/model.
     */
    class INode
    {
    public:
        /**
         * Virtual Destructor
         */
        virtual ~INode() = default;

        /**
         * Evaluates the value of this subtree.
         * @return the primitate value for a variable, or the return 
         * value for a function
         */
        virtual double Evaluate() const = 0;

        /**
         * @return the string representation of this subtree.
         */
        virtual std::string ToString() const = 0;
        
        /**
         * Transfers all children of this node to the other node
         * @param other The node to move the children to
         * @return true if the transfer completes successfully
         */
        virtual bool MoveChildrenTo(std::unique_ptr<INode>& other) = 0;

        /**
         * Add a child node to this node
         */
        virtual bool AddChild(std::unique_ptr<INode> child) = 0;

        /**
         * @return The number of immediate children of this node
         */
        virtual int NumberOfChildren() const = 0;

        /**
         * @return a reference to the direct descendents of this INode
         */
        virtual const std::vector<std::unique_ptr<INode>>& GetChildren() const = 0;

        /**
         * @return The maximum allowed immediate children of this node
         */
        virtual int MaxChildren() const = 0;

        /**
         * Gets the nubmer of nodes in the chromosome tree (self included)
         * @return the size of the tree
         */
        virtual int Size() const = 0;

        /**
         * @return a copy of this (sub)tree
         */
        virtual std::unique_ptr<INode> Clone() const = 0;

        /**
         * returns true if the number of children is less than the minimum required
         */
        virtual bool LacksBreadth() const { return false; }

        /**
         * Returns a reference to the unique pointer at the specified index, pre-order
         * @param index The node index (in-order traversal). 
         * @return A reference to the node unique_ptr
         * @throws an exception if index exceeds is >= the tree size
         */
        virtual std::unique_ptr<INode>& Get(int index, std::unique_ptr<INode>& ptr) = 0;

        /**
         * Compares two INodes to determine if they are the same (ignoring children)
         * @param other The other INode to compare against
         * @return true if they are the same.
         */
        bool IsEquivalent(const INode& other) const { return GetSymbol() == other.GetSymbol(); }

    protected:
        /**
         * @retun the symbol for the INode
         */
        virtual std::string GetSymbol() const = 0;

        double m_fitness = 0.0; ///< The fitness of this node
    };
}

#endif
