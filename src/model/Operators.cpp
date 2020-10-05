#include "Operators.h"

#include <memory>
#include "Terminal.h"
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
        int maxIndex = static_cast<int>(size-1);
        return RandInt.GetInRange(0, maxIndex);
    }

    /**
     * @param gene The S-expression gene to inspect
     * @return true if the gene is a Terminal (not a Function)
     */
    auto isTerminal = [](const std::unique_ptr<Model::INode>& gene)
    {
        return gene->MaxChildren() == 0;
    };
}

namespace Model { namespace Operators 
{
    void SetSeed(int seed)
    {
        RandInt.SetSeed(seed);
    }

    void Mutate(std::unique_ptr<INode>& chromosome, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables)
    {
        auto randomTerminalMutation = [&](std::unique_ptr<INode>& gene) -> void
        {
            // Prevents mutation to the same terminal, if there are 2+ terminals available.
            // TODO: this could likely be done in a more efficient manner.
            std::vector<double*> tTypes(variables);
            while (!tTypes.empty())
            {
                int i = RandomIndex(tTypes.size());
                auto newTerminal = FunctionFactory::Create(tTypes[i]);
                if (!gene->IsEquivalent(*newTerminal))
                {
                    gene.swap(newTerminal);
                    break;
                }
                auto itr = tTypes.begin() + i;
                tTypes.erase(itr);
            }
        };

        auto randomFunctionMutation = [&](std::unique_ptr<INode>& gene) -> void
        {
            // copy function types, such that we can remove types and prevent an infinite loop 
            // in the case where no function in the vector can handle the number of children.
            // This does not (and cannot) guarantee that the function will change, since we 
            // can't guarantee that a suitable replacement will be available to mutate to.
            std::vector<FunctionType> fTypes(allowedFunctions);

            while (!fTypes.empty())
            {
                int i = RandomIndex(fTypes.size());
                auto newFunction = FunctionFactory::Create(fTypes[i]);

                if (gene->NumberOfChildren() < newFunction->MaxChildren()) // TODO: and children > MinChildren?
                {
                    gene->MoveChildrenTo(newFunction); // transfer sub tree
                    gene.swap(newFunction);
                    break;
                }

                auto itr = fTypes.begin() + i; 
                fTypes.erase(itr);
            }
            // else the mutation fails
        };
        
        // Randomly select a node in the chromosome tree 
        int index = RandInt.GetInRange(0, chromosome->Size()-1);
        auto& gene = chromosome->Get(index, chromosome);

        if (isTerminal(gene))
        {
            randomTerminalMutation(gene);
        }
        else
        {
            randomFunctionMutation(gene);
        }
    }

    void Crossover(std::unique_ptr<INode>& left, std::unique_ptr<INode>& right)
    {
        int leftSize = left->Size();
        int rightSize = right->Size();
        if (leftSize == 1 && rightSize == 1)
        {
            left.swap(right);
        }

        // Pick a random node in left
        if (leftSize == 1)
        {
            left.swap(right->Get(RandInt.GetInRange(1, rightSize-1), right));
            return;
        }
        else if (rightSize == 1)
        {
            right.swap(left->Get(RandInt.GetInRange(1, leftSize-1), left));
        }
        else
        {
            int leftIndex = RandInt.GetInRange(1, leftSize-1);
            int rightIndex = RandInt.GetInRange(1, rightSize-1);
            auto& leftSubtree = left->Get(leftIndex, left);
            leftSubtree.swap(right->Get(rightIndex, right));
        }
    }

    std::unique_ptr<INode> CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables)
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
            auto isFunction = RandInt.GetInRange(0, 1) == 0;
            std::unique_ptr<INode> newNode;
            if (isFunction)
            {
                // randomly select a function type
                index = RandomIndex(allowedFunctions.size());
                newNode = FunctionFactory::Create(allowedFunctions[index]); 
                functions.push_back(newNode.get());
            }
            else 
            {
                // randomly select a variable
                index = RandomIndex(variables.size());
                newNode = FunctionFactory::Create(variables[index]);
            }
            
            // get a random position to insert (must be a Function)
            int insertIndex = -1;
            do
            {
                insertIndex = RandInt.GetInRange(0, root->Size()-1);
            } 
            while (insertIndex != 0 && isTerminal(root->Get(insertIndex, root)) );

            // add the new node to the random position in the tree
            if (!root->Get(insertIndex, root)->AddChild(std::move(newNode)) && isFunction)
            {
                functions.pop_back(); // remove from cache if failed
            }
        }
        
        // Make sure none of the leaf nodes are functions, and that functions have
        // their minimum number of children.
        for (auto& func : functions)
        {
            while (func->NumberOfChildren() == 0 || func->LacksBreadth())
            {
                // add a variable
                index = RandomIndex(variables.size());
                func->AddChild(FunctionFactory::Create(variables[index]));
            }
        }
        return root;
    }
}}
