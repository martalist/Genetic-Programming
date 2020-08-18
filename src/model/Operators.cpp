#include "Operators.h"

#include <memory>
#include "../utils/UniformRandomGenerator.h"

namespace
{
    /**
     * Gets a random index into a collection of the specified size
     * @param size The size of the collection
     */
    int RandomIndex(size_t size)
    {
        static Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>> randomInteger(0, 1000);
        int maxIndex = static_cast<int>(--size);
        return randomInteger.GetInRange(0, maxIndex);
    }
}

namespace Model { namespace Operators 
{
    void Mutate(INode& chromosome)
    {
        // the decision on whether to mutate is made in Program (not here)
        
        // TODO
        // Randomly select a node in the chromosome tree
        // if a function
            // randomly select a new function
            // (the new function must be compatible with the number of children)
            // replace by altering parent/child pointers
        // else if a variable
            // randomly select a new variable
    }

    void Crossover(INode& left, INode& right)
    {
        // TODO
    }

    std::unique_ptr<INode> CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions)
    {
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
