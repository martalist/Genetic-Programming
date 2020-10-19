#include "Chromosome.h"

#include <algorithm>
#include <cmath>
#include "FunctionFactory.h"
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

    /**
     * Adds terminals to a function to ensure it has a valid number of children.
     * @pre This should only be called on Function objects.
     * @param func The function to fill.
     */
    void FillFunction(Model::INode* func, const std::vector<double*>& variables)
    {
        while (func->LacksBreadth())
        {
            auto index = RandomIndex(variables.size());
            func->AddChild(Model::FunctionFactory::Create(variables[index]));
        }
    }
}

namespace Model
{
    void IChromosome::SetSeed(int seed)
    {
        RandInt.SetSeed(seed);
    }

    Chromosome::Chromosome(int targetSize, 
            const std::vector<FunctionType>& allowedFunctions, 
            const std::vector<double*>& variables,
            const std::vector<std::vector<double>>& fitnessCases, 
            std::vector<double>& terminals, 
            double parsimonyCoefficient)
        : m_tree(CreateRandomChromosome(targetSize, allowedFunctions, variables))
        , m_size(m_tree->Size())
        , m_fitness(CalculateFitness(fitnessCases, terminals))
        , m_weightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    }

    Chromosome::Chromosome(IChromosome::INodePtr tree)
        : m_tree(std::move(tree)) 
        , m_size(m_tree->Size())
    {
    }

    Chromosome::Chromosome(IChromosome::INodePtr& tree, const std::vector<std::vector<double>>& fitnessCases, 
            std::vector<double>& terminals, double parsimonyCoefficient)
        : m_tree(std::move(tree)) 
        , m_size(m_tree->Size())
        , m_fitness(CalculateFitness(fitnessCases, terminals))
        , m_weightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    } 

    Chromosome::Chromosome(IChromosome::INodePtr& tree, double fitness, double parsimonyCoefficient)
        : m_tree(std::move(tree)) 
        , m_size(m_tree->Size())
        , m_fitness(fitness)
        , m_weightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    } 

    bool Chromosome::operator<(const IChromosome& right) const
    {
        const auto rhs = dynamic_cast<const Chromosome*>(&right);
        return m_weightedFitness < rhs->m_weightedFitness;
    }

    double Chromosome::CalculateFitness(const std::vector<std::vector<double>>& fitnessCases, std::vector<double>& terminals) const
    {
        double sumOfErrors = 0.0;
        for (const auto& fCase : fitnessCases) // FitnessCases are the training set
        {
            // load up the terminals for this fitness case
            std::copy(fCase.begin(), fCase.end()-1, terminals.begin());

            // calculate the fitness (absolute error) for this fitness case
            auto returnVal = m_tree->Evaluate();

            // add to the tally
            sumOfErrors += std::abs(returnVal - fCase.back());
        }
        return sumOfErrors / fitnessCases.size(); // mean absolute error
    }

    double Chromosome::CalculateWeightedFitness(double parsimonyCoefficient) const
    {
        return m_fitness + parsimonyCoefficient * m_size;
    }

    int Chromosome::Size() const
    {
        return m_size;
    }

    double Chromosome::Fitness() const
    {
        return m_fitness;
    }

    std::unique_ptr<IChromosome> Chromosome::Clone() const
    {
        return std::make_unique<Chromosome>(*this);
    }

    Chromosome::Chromosome(const Chromosome& other)
    {
        m_tree = other.m_tree->Clone();
        m_size = other.m_size;
        m_fitness = other.m_fitness;
        m_weightedFitness = other.m_weightedFitness;
    }

    void Chromosome::Mutate(const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables)
    {
        auto randomTerminalMutation = [&](std::unique_ptr<INode>& gene) -> void
        {
            if (RandInt.GetInRange(0,1) && !allowedFunctions.empty()) // mutate to a function
            {
                int i = RandomIndex(allowedFunctions.size());
                auto func = FunctionFactory::Create(allowedFunctions[i]);
                FillFunction(func.get(), variables);
                gene.swap(func);
            }
            else // mutate to a terminal
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

                if (gene->NumberOfChildren() <= newFunction->MaxChildren()) // TODO: and children > MinChildren?
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
        int index = RandInt.GetInRange(0, m_size-1);
        auto& gene = m_tree->Get(index, m_tree);

        if (isTerminal(gene))
        {
            randomTerminalMutation(gene);
        }
        else
        {
            randomFunctionMutation(gene);
        }

        // update the cached size of the chromosome
        SetSize(m_tree->Size());
    }

    void Chromosome::HoistMutate()
    {
        // get the target gene from within chromosome
        if (Size() == 1) 
        {
            return; // chromosome is a terminal; nothing to hoist
        }

        // get the target gene (that we'll hoist into)
        auto index = RandInt.GetInRange(0, Size()-1);
        auto& target = index == 0 ? m_tree : m_tree->Get(index, m_tree);

        int targetSize = target->Size();
        if (targetSize == 1)
        {
            return; // target is a terminal, so there is no subtree to hoist
        }

        // get the subtree to hoist, and swap them
        index = RandInt.GetInRange(0, targetSize-1);
        auto& toHoist = index == 0 ? target : target->Get(index, target);
        target = std::move(toHoist);

        // update the cached size of the chromosome
        SetSize(m_tree->Size());
    }

    void Chromosome::Crossover(IChromosome& right)
    {
        auto rhs = dynamic_cast<Chromosome*>(&right);
        if (Size() == 1 && rhs->Size() == 1)
        {
            // do nothing; swapping at the base yields two S-expressions the same
            return;
        }

        // Pick a random node in left
        if (Size() == 1)
        {
            m_tree.swap(rhs->GetTree()->Get(RandInt.GetInRange(1, rhs->Size()-1), rhs->GetTree()));
        }
        else if (rhs->Size() == 1)
        {
            rhs->GetTree().swap(m_tree->Get(RandInt.GetInRange(1, Size()-1), m_tree));
        }
        else
        {
            int leftIndex = RandInt.GetInRange(1, Size()-1);
            int rhsIndex = RandInt.GetInRange(1, rhs->Size()-1);
            auto& leftSubtree = m_tree->Get(leftIndex, m_tree);
            leftSubtree.swap(rhs->GetTree()->Get(rhsIndex, rhs->GetTree()));
        }

        SetSize(m_tree->Size());
        rhs->SetSize(rhs->GetTree()->Size());
    }

    void Chromosome::SetSize(int size)
    {
        m_size = size;
    }

    IChromosome::INodePtr& Chromosome::GetTree()
    {
        return m_tree;
    }

    const IChromosome::INodePtr& Chromosome::GetTree() const
    {
        return m_tree;
    }

    std::unique_ptr<INode> Chromosome::CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables)
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
            FillFunction(func, variables);
        }
        return root;
    }
}
