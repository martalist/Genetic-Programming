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
        int maxIndex = static_cast<int>(--size);
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
        auto randomMutation = [&](std::unique_ptr<INode>& gene) -> void
        {
            if (isTerminal(gene))
            {
                int i = RandomIndex(variables.size());
                gene = std::make_unique<Terminal>(variables[i]);
            }
            else
            {
                std::unique_ptr<INode> newFunction = nullptr;

                // copy function types, such that we can remove types and prevent an infinite loop 
                // in the case where no function in the vector can handle the number of children.
                std::vector<FunctionType> fTypes(allowedFunctions);
                do
                {
                    int i = RandomIndex(fTypes.size());
                    newFunction = FunctionFactory::Create(fTypes[i]);
                    auto itr = fTypes.begin() + i;
                    fTypes.erase(itr);
                } while(gene->NumberOfChildren() > newFunction->MaxChildren() && !fTypes.empty());

                if (!fTypes.empty())
                {
                    gene->MoveChildrenTo(newFunction); // transfer sub tree
                    gene.swap(newFunction);
                }
                // else the mutation fails
            }
        };
        
        // Randomly select a node in the chromosome tree 
        int index = RandInt.GetInRange(0, chromosome->Size()-1);
        auto& gene = chromosome->Get(index, chromosome);
        randomMutation(gene);
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
