#ifndef INode_H
#define INode_H

#include <memory>
#include <string>

namespace Model
{
    /**
     * An interface Node of the genetic programming tree/model.
     */
    class INode
    {
    public:
        /**
         * Copy Constructor TODO
         */
        // virtual INode(const INode& other) = 0;

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
         * @return The maximum allowed immediate children of this node
         */
        virtual int MaxChildren() const = 0;

        /**
         * Gets the nubmer of nodes in the chromosome tree (self included)
         * @return the size of the tree
         */
        virtual int Size() const = 0;

        /**
         * @return true if this node is a variable, else false (i.e. a function)
         */
        virtual bool IsTerminal() const = 0; // TODO: consider removing - MaxChildren can answer this question

        /*
         * Assignment operator
         */
        // virtual INode& operator=(const INode& rhs) = 0;

        /**
         * Returns a reference to the unique pointer at the specified index, pre-order
         * @param index The node index (in-order traversal). 
         * @return A reference to the node unique_ptr
         * @throws an exception if index exceeds is >= the tree size
         */
        virtual std::unique_ptr<INode>& Get(int index, std::unique_ptr<INode>& ptr) = 0;
    };
}

#endif
