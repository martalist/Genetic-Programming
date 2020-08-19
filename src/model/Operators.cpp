#include "Operators.h"

#include <memory>
#include "../utils/UniformRandomGenerator.h"

namespace
{
    /**
     * A random integer generator for reuse in the methods below
     */
    Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>> RandInt(0, 1);

    /**
     * Gets a random index into a collection of the specified size
     * @param size The size of the collection
     */
    int RandomIndex(size_t size)
    {
        int maxIndex = static_cast<int>(--size);
        return RandInt.GetInRange(0, maxIndex);
    }
}

namespace Model { namespace Operators 
{
    void Mutate(INode& chromosome)
    {
        // Randomly select a node in the chromosome tree 
        int size = chromosome.Size();
        int index = size == 1 ? 0 : RandInt.GetInRange(1, size - 1);
        // get it out of the tree
            // TODO: needs us to be able to iterate over the tree
        // if a function
            // randomly select a new function
            // (the new function must be compatible with the number of children)
            // replace by altering parent/child pointers
        // else if a variable
            // randomly select a new variable
    }

    void Crossover(INode& left, INode& right)
    {
        int lSize = left.Size();
        int rSize = right.Size();
        if (lSize == 1 && rSize == 1)
        {
            return; // nothing to do here
        }

        // TODO
        // Pick a random node in left
        int l = lSize == 1 ? 0 : RandInt.GetInRange(1, lSize);
            // get a pointer/reference to it
        // Pick a random node in right
        int r = rSize == 1 ? 0 : RandInt.GetInRange(1, rSize);
            // get a pointer/reference to it
        // swap them (pointers)
    }

    std::unique_ptr<INode> CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions)
    {
        // TODO: this needs to be checked, and should be tested!!
        // start with a randomly selected function
        int index = RandomIndex(allowedFunctions.size());
        auto root = FunctionFactory::Create(allowedFunctions[index]);

        // keep a local list of function nodes, for easy reference
        std::vector<INode*> functions;
        functions.push_back(root.get());

        for (int count = 1; count < targetSize; ++count)
        {
            // Randomly choose a new function/variable
            auto newNode = FunctionFactory::Create(FunctionType::Addition);

            // add the new node to the randomly selected one
            auto func = functions[ RandomIndex(functions.size()) ];
            func->AddChild(std::move(newNode));
        }
        
        // Make sure none of the leaf nodes are functions
        for (auto& func : functions)
        {
            if (func->NumberOfChildren() == 0)
            {
                // add a variable
            }
        }
        return std::move(root);
    }
}}
