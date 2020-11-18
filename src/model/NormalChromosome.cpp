#include "NormalChromosome.h"

#include <algorithm>
#include <cmath>
#include "../PopulationParams.h"
#include "FunctionFactory.h"

namespace Model
{
    NormalChromosome::NormalChromosome(int targetSize, 
            const std::vector<FunctionType>& allowedFunctions, 
            const std::vector<double*>& variables,
            const TrainingData& fitnessCases, 
            std::vector<double>& terminals, 
            double parsimonyCoefficient,
            Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>>& rand,
            FunctionFactory& funcFactory)
        : IChromosome::IChromosome(rand, funcFactory)
        , m_tree(CreateRandomChromosome(targetSize, allowedFunctions, variables))
        , m_size(m_tree->Size())
        , m_fitness(CalculateFitness(fitnessCases, terminals))
        , m_weightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    }

    NormalChromosome::NormalChromosome(IChromosome::INodePtr tree,
            Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>>& rand,
            FunctionFactory& funcFactory)
        : IChromosome::IChromosome(rand, funcFactory)
        , m_tree(std::move(tree)) 
        , m_size(m_tree->Size())
    {
    }

    NormalChromosome::NormalChromosome(IChromosome::INodePtr tree, const TrainingData& fitnessCases, 
            std::vector<double>& terminals, double parsimonyCoefficient,
            Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>>& rand,
            FunctionFactory& funcFactory)
        : IChromosome::IChromosome(rand, funcFactory)
        , m_tree(std::move(tree)) 
        , m_size(m_tree->Size())
        , m_fitness(CalculateFitness(fitnessCases, terminals))
        , m_weightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    } 

    NormalChromosome::NormalChromosome(IChromosome::INodePtr& tree, double fitness, 
            double parsimonyCoefficient,
            Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>>& rand,
            FunctionFactory& funcFactory)
        : IChromosome::IChromosome(rand, funcFactory)
        , m_tree(std::move(tree)) 
        , m_size(m_tree->Size())
        , m_fitness(fitness)
        , m_weightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    } 

    bool NormalChromosome::operator<(const IChromosome& right) const
    {
        const auto rhs = dynamic_cast<const NormalChromosome*>(&right);
        return m_weightedFitness < rhs->m_weightedFitness;
    }

    double NormalChromosome::CalculateFitness(const TrainingData& fitnessCases, std::vector<double>& terminals)
    {
        double sumOfErrors = 0.0;
        int columns = static_cast<int>(terminals.size()) + 1; // columns in csv file, incl dependent variable
        int totalCases = fitnessCases.Len / columns;       // rows in the csv file

        for (int i = 0; i < totalCases; ++i)
        {
            // load up the terminals for this fitness case
            auto begin = fitnessCases.Cases + i*columns;
            auto end = begin + columns - 1;
            std::copy(begin, end, terminals.begin());

            // calculate the fitness (absolute error) for this fitness case
            auto returnVal = m_tree->Evaluate();

            // add to the tally
            sumOfErrors += std::abs(returnVal - *end);
        }
        return sumOfErrors / totalCases; // mean absolute error
    }

    double NormalChromosome::CalculateWeightedFitness(double parsimonyCoefficient) const
    {
        return m_fitness + parsimonyCoefficient * m_size;
    }

    int NormalChromosome::Size() const
    {
        return m_size;
    }

    double NormalChromosome::Fitness() const
    {
        return m_fitness;
    }

    std::unique_ptr<IChromosome> NormalChromosome::Clone() const
    {
        return std::make_unique<NormalChromosome>(*this);
    }

    NormalChromosome::NormalChromosome(const NormalChromosome& other)
        : IChromosome::IChromosome(other.m_randInt, other.m_funcFactory)
    {
        m_tree = other.m_tree->Clone();
        m_size = other.m_size;
        m_fitness = other.m_fitness;
        m_weightedFitness = other.m_weightedFitness;
    }

    void NormalChromosome::Mutate(const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables)
    {
        auto randomTerminalMutation = [&](std::unique_ptr<INode>& gene) -> void
        {
            if (m_randInt.GetInRange(0,1) && !allowedFunctions.empty()) // mutate to a function
            {
                int i = RandomIndex(allowedFunctions.size());
                auto func = m_funcFactory.Create(allowedFunctions[i]);
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
                    auto newTerminal = m_funcFactory.Create(tTypes[i]);
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
                auto newFunction = m_funcFactory.Create(fTypes[i]);

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
        int index = m_randInt.GetInRange(0, m_size-1);
        auto& gene = m_tree->Get(index, m_tree);

        if (IsTerminal(gene))
        {
            randomTerminalMutation(gene);
        }
        else
        {
            randomFunctionMutation(gene);
        }

        // update the cached size of the chromosome
        SetSize();
    }

    void NormalChromosome::HoistMutate()
    {
        // get the target gene from within chromosome
        if (Size() == 1) 
        {
            return; // chromosome is a terminal; nothing to hoist
        }

        // get the target gene (that we'll hoist into)
        auto index = m_randInt.GetInRange(0, Size()-1);
        auto& target = index == 0 ? m_tree : m_tree->Get(index, m_tree);

        int targetSize = target->Size();
        if (targetSize == 1)
        {
            return; // target is a terminal, so there is no subtree to hoist
        }

        // get the subtree to hoist, and swap them
        index = m_randInt.GetInRange(0, targetSize-1);
        auto& toHoist = index == 0 ? target : target->Get(index, target);
        target = std::move(toHoist);

        // update the cached size of the chromosome
        SetSize();
    }

    void NormalChromosome::Crossover(IChromosome& right)
    {
        auto rhs = dynamic_cast<NormalChromosome*>(&right);
        if (Size() == 1 && rhs->Size() == 1)
        {
            // do nothing; swapping at the base yields two S-expressions the same
            return;
        }

        // Pick a random node in left
        if (Size() == 1)
        {
            m_tree.swap(rhs->GetTree()->Get(m_randInt.GetInRange(1, rhs->Size()-1), rhs->GetTree()));
        }
        else if (rhs->Size() == 1)
        {
            rhs->GetTree().swap(m_tree->Get(m_randInt.GetInRange(1, Size()-1), m_tree));
        }
        else
        {
            int leftIndex = m_randInt.GetInRange(1, Size()-1);
            int rhsIndex = m_randInt.GetInRange(1, rhs->Size()-1);
            auto& leftSubtree = m_tree->Get(leftIndex, m_tree);
            leftSubtree.swap(rhs->GetTree()->Get(rhsIndex, rhs->GetTree()));
        }

        SetSize();
        rhs->SetSize();
    }

    void NormalChromosome::SetSize()
    {
        m_size = m_tree->Size();
    }

    IChromosome::INodePtr& NormalChromosome::GetTree()
    {
        return m_tree;
    }

    const IChromosome::INodePtr& NormalChromosome::GetTree() const
    {
        return m_tree;
    }

    std::unique_ptr<INode> NormalChromosome::CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables)
    {
        // start with a randomly selected function
        int index = RandomIndex(allowedFunctions.size());
        auto root = m_funcFactory.Create(allowedFunctions[index]);

        // keep a local list of function nodes, for easy reference
        std::vector<INode*> functions;
        functions.push_back(root.get());

        for (int count = 1; count < targetSize; ++count)
        {
            // Randomly choose a new function/variable
            auto isFunction = m_randInt.GetInRange(0, 1) == 0;
            std::unique_ptr<INode> newNode;
            if (isFunction)
            {
                // randomly select a function type
                index = RandomIndex(allowedFunctions.size());
                newNode = m_funcFactory.Create(allowedFunctions[index]); 
                functions.push_back(newNode.get());
            }
            else 
            {
                // randomly select a variable
                index = RandomIndex(variables.size());
                newNode = m_funcFactory.Create(variables[index]);
            }
            
            // get a random position to insert (must be a Function)
            int insertIndex = -1;
            do
            {
                insertIndex = m_randInt.GetInRange(0, root->Size()-1);
            } 
            while (insertIndex != 0 && IsTerminal(root->Get(insertIndex, root)) );

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

    std::string NormalChromosome::ToString() const
    {
        return m_tree->ToString();
    }

    void NormalChromosome::Forecast(const TrainingData& fitnessCases, std::vector<double>& terminals, double* predictions, int length) const
    {
        throw std::invalid_argument("Prediction is not yet implemented for ChromosomeType::Normal.");
    }

    void NormalChromosome::Predict(std::vector<double>& predictionCases, std::vector<double>& terminals, int cutoff) const
    {
        throw std::invalid_argument("Prediction is not yet implemented for ChromosomeType::Normal.");
    }
}
