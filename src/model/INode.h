#ifndef INode_H
#define INode_H

#include <memory>
#include <string>

namespace Model
{
    
    class INode;
    typedef std::vector<std::unique_ptr<INode>> ChildNodes;

    /**
     * An interface Node of the genetic programming tree/model.
     */
    class INode
    {
    public:
        /**
         * Copy Constructor
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

        /*
         * Assignment operator
         */
        // virtual INode& operator=(const INode& rhs) = 0;
    };
}

#endif
