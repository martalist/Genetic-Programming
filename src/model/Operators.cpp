#include "Operators.h"

#include <memory>
#include "Variable.h"
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
    void Mutate(std::unique_ptr<INode>& chromosome, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*> variables)
    {
        auto randomMutation = [&](std::unique_ptr<INode>& gene)
        {
            if (gene->IsVariable())
            {
                int i = RandomIndex(variables.size());
                gene = std::make_unique<Variable>(*variables[i]);
            }
            else
            {
                std::unique_ptr<INode> newFunction = nullptr;
                do
                {
                    int i = RandomIndex(allowedFunctions.size());
                    newFunction = FunctionFactory::Create(allowedFunctions[i]);
                } while(gene->NumberOfChildren() > newFunction->MaxChildren());

                gene->SwapWith(std::move(newFunction));
            }
        };
        // Randomly select a node in the chromosome tree 
        int size = chromosome->Size();
        int index = RandInt.GetInRange(0, size-1);
        if (index == 0)
        {
            randomMutation(chromosome);
        }
        else
        {
            auto& gene = chromosome->Get(index);
            randomMutation(gene);
        }
        // get it out of the tree
            // TODO: needs us to be able to iterate over the tree
        // if a function
            // randomly select a new function
            // (the new function must be compatible with the number of children)
            // replace by altering parent/child pointers
        // else if a variable
            // randomly select a new variable
    }

    void Crossover(std::unique_ptr<INode>& left, std::unique_ptr<INode>& right)
    {
        int leftSize = left->Size();
        int rightSize = right->Size();
        if (leftSize == 1 && rightSize == 1)
        {
            return; // nothing to do here
        }

        // Pick a random node in left
        if (leftSize == 1)
        {
            left.swap(right->Get(RandInt.GetInRange(1, rightSize-1)));
            return;
        }
        else if (rightSize == 1)
        {
            right.swap(left->Get(RandInt.GetInRange(1, leftSize-1)));
        }
        else
        {
            int leftIndex = RandInt.GetInRange(1, leftSize-1);
            int rightIndex = RandInt.GetInRange(1, rightSize-1);
            auto& leftSubtree = left->Get(leftIndex);
            leftSubtree.swap(right->Get(rightIndex));
        }
    }

    std::unique_ptr<INode> CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*> variables)
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
