#include "TimeSeriesChromosome.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include "../PopulationParams.h"
#include "FunctionFactory.h"
#include "ChromosomeUtil.h"

namespace Model
{
    using namespace ChromosomeUtil;

    TimeSeriesChromosome::TimeSeriesChromosome(int targetSize, 
            const std::vector<FunctionType>& allowedFunctions, 
            const std::vector<double*>& variables,
            const TrainingData& fitnessCases, 
            std::vector<double>& terminals, 
            double parsimonyCoefficient)
        : m_tree(CreateRandomChromosome(targetSize, allowedFunctions, variables))
        , m_coefficients(m_tree->NumberOfChildren()+1)
        , m_size(m_tree->Size())
        , m_fitness(CalculateFitness(fitnessCases, terminals))
        , m_weightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    }

    // TimeSeriesChromosome::TimeSeriesChromosome(IChromosome::INodePtr tree)
        // : m_tree(std::move(tree)) 
        // , m_coefficients(m_tree->NumberOfChildren()+1)
        // , m_size(m_tree->Size())
    // {
    // }

    TimeSeriesChromosome::TimeSeriesChromosome(IChromosome::INodePtr tree, const TrainingData& fitnessCases, 
            std::vector<double>& terminals, double parsimonyCoefficient)
        : m_tree(std::move(tree)) 
        , m_coefficients(m_tree->NumberOfChildren()+1)
        , m_size(m_tree->Size())
        , m_fitness(CalculateFitness(fitnessCases, terminals))
        , m_weightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    } 

    // TimeSeriesChromosome::TimeSeriesChromosome(IChromosome::INodePtr tree, double fitness, double parsimonyCoefficient)
        // : m_tree(std::move(tree)) 
        // , m_coefficients(m_tree->NumberOfChildren()+1)
        // , m_size(m_tree->Size())
        // , m_fitness(fitness)
        // , m_weightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    // {
    // } 

    std::unique_ptr<IChromosome> TimeSeriesChromosome::Clone() const
    {
        return std::make_unique<TimeSeriesChromosome>(*this); 
    }

    TimeSeriesChromosome::TimeSeriesChromosome(const TimeSeriesChromosome& other)
    {
        m_tree = other.m_tree->Clone();
        m_coefficients = other.m_coefficients;
        m_size = other.m_size;
        m_fitness = other.m_fitness;
        m_weightedFitness = other.m_weightedFitness;
    }

    bool TimeSeriesChromosome::operator<(const IChromosome& right) const
    {
        const auto rhs = dynamic_cast<const TimeSeriesChromosome*>(&right);
        return m_weightedFitness < rhs->m_weightedFitness;
    }

    double TimeSeriesChromosome::CalculateFitness(const TrainingData& fitnessCases, std::vector<double>& terminals)
    {
        double sumOfSqErrors = 0.0;
        int lag = terminals.size();
        int totalCases = fitnessCases.Len - lag;

        Eigen::MatrixXd W(totalCases, m_coefficients.size());
        Eigen::VectorXd Y(totalCases);

        // Iterate over the fitnessCases vector to build the W matrix and Y vector.
        for (int i = 0; i < totalCases; ++i)
        {
            // load up the terminals for this fitness case
            for (size_t j = 0; j < lag; ++j)
            {
                terminals[j] = fitnessCases.Cases[i+j];
            }

            Y(i) = fitnessCases.Cases[i+lag]; // add to Y vector
            W(i,0) = 1.0; // first column is always 1
            if (m_size != 1) // check that this isn't just a terminal
            {
                const auto& modelTerms = m_tree->GetChildren();
                assert(m_coefficients.size()-1 == modelTerms.size()); // sanity check

                for (size_t j = 0; j < modelTerms.size(); ++j)
                {
                    W(i,j+1) = modelTerms[j]->Evaluate();
                }
            }
        }

        // @see https://eigen.tuxfamily.org/dox-devel/group__LeastSquares.html
        // m_coefficients = W.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(Y);
        m_coefficients = W.colPivHouseholderQr().solve(Y);

        auto errors = Y - (W*m_coefficients); // \hat{Y} = W*beta
        sumOfSqErrors = errors.dot(errors);
        return std::sqrt(sumOfSqErrors/(totalCases)); // Standard Error
    }

    double TimeSeriesChromosome::CalculateWeightedFitness(double parsimonyCoefficient) const
    {
        return m_fitness + parsimonyCoefficient * m_size;
    }

    int TimeSeriesChromosome::Size() const
    {
        return m_size;
    }

    double TimeSeriesChromosome::Fitness() const
    {
        return m_fitness;
    }

    void TimeSeriesChromosome::Mutate(const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables)
    {
        auto randomTerminalMutation = [&](std::unique_ptr<INode>& gene) -> void
        {
            if (RandInt().GetInRange(0,1) && !allowedFunctions.empty()) // mutate to a function
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
        int index = RandInt().GetInRange(0, m_size-1);
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

    void TimeSeriesChromosome::HoistMutate()
    {
        // get the target gene from within chromosome
        if (Size() == 1) 
        {
            return; // chromosome is a terminal; nothing to hoist
        }

        // get the target gene (that we'll hoist into)
        auto index = RandInt().GetInRange(0, Size()-1);
        auto& target = index == 0 ? m_tree : m_tree->Get(index, m_tree);

        int targetSize = target->Size();
        if (targetSize == 1)
        {
            return; // target is a terminal, so there is no subtree to hoist
        }

        // get the subtree to hoist, and swap them
        index = RandInt().GetInRange(0, targetSize-1);
        auto& toHoist = index == 0 ? target : target->Get(index, target);
        target = std::move(toHoist);

        // update the cached size of the chromosome
        SetSize();
    }

    void TimeSeriesChromosome::Crossover(IChromosome& right)
    {
        auto rhs = dynamic_cast<TimeSeriesChromosome*>(&right);
        if (Size() == 1 && rhs->Size() == 1)
        {
            // do nothing; swapping at the base yields two S-expressions the same
            return;
        }

        // Pick a random node in left
        if (Size() == 1)
        {
            m_tree.swap(rhs->GetTree()->Get(RandInt().GetInRange(1, rhs->Size()-1), rhs->GetTree()));
        }
        else if (rhs->Size() == 1)
        {
            rhs->GetTree().swap(m_tree->Get(RandInt().GetInRange(1, Size()-1), m_tree));
        }
        else
        {
            int leftIndex = RandInt().GetInRange(1, Size()-1);
            int rhsIndex = RandInt().GetInRange(1, rhs->Size()-1);
            auto& leftSubtree = m_tree->Get(leftIndex, m_tree);
            leftSubtree.swap(rhs->GetTree()->Get(rhsIndex, rhs->GetTree()));
        }

        SetSize();
        rhs->SetSize();
    }

    void TimeSeriesChromosome::SetSize()
    {
        m_size = m_tree->Size();
        m_coefficients.resize(m_tree->NumberOfChildren()+1);
    }

    IChromosome::INodePtr& TimeSeriesChromosome::GetTree()
    {
        return m_tree;
    }

    const IChromosome::INodePtr& TimeSeriesChromosome::GetTree() const
    {
        return m_tree;
    }

    std::unique_ptr<INode> TimeSeriesChromosome::CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables)
    {
        // start with an addition function, since this forms the basis of the autoregressive model
        auto root = FunctionFactory::Create(FunctionType::Addition);

        // keep a local list of function nodes, for easy reference
        std::vector<INode*> functions;
        functions.push_back(root.get());

        for (int count = 1; count < targetSize; ++count)
        {
            // Randomly choose a new function/variable
            auto isFunction = ( RandInt().GetInRange(0, 1) == 0 );
            std::unique_ptr<INode> newNode;
            if (isFunction)
            {
                // randomly select a function type
                int index = RandomIndex(allowedFunctions.size());
                newNode = FunctionFactory::Create(allowedFunctions[index]); 
                functions.push_back(newNode.get());
            }
            else 
            {
                // randomly select a variable
                int index = RandomIndex(variables.size());
                newNode = FunctionFactory::Create(variables[index]);
            }
            
            // get a random position to insert (must be a Function)
            int insertIndex = -1;
            do
            {
                insertIndex = RandInt().GetInRange(0, root->Size()-1);
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

    std::string TimeSeriesChromosome::ToString() const
    {
        std::stringstream output;
        output << m_coefficients(0) << " + ";
        if (m_size != 1) // not a terminal
        {
            const auto& terms = m_tree->GetChildren();
            int i = 0;
            for (const auto& term : terms)
            {
                output << "(" << m_coefficients(++i) << " * " << term->ToString() << ")";
                output << ((i < terms.size()) ? " + " : "");
            }
        }
        else
        {
            output << m_tree->ToString();
        }
        return output.str();
    }

    void TimeSeriesChromosome::Forecast(const TrainingData& fitnessCases, std::vector<double>& terminals, double* predictions, int length) const
    {
        int lag = terminals.size();

        // Iterate over the fitnessCases vector to build the W matrix and Y vector.
        for (int i = 0; i < length; ++i)
        {
            // load up the terminals for this fitness case
            for (size_t j = 0; j < lag; ++j)
            {
                auto targetIndex = fitnessCases.Len + i + j - lag;
                // terminals[j] = targetIndex < fitnessCases.size() 
                    // ? fitnessCases[targetIndex] 
                    // : predictions[i+j-lag];
                if (targetIndex < fitnessCases.Len)
                {
                    terminals[j] = fitnessCases.Cases[targetIndex] ;
                }
                else
                {
                    terminals[j] = predictions[i+j-lag];
                }
            }

            predictions[i] = m_coefficients[0];
            if (m_size != 1) // check that this isn't just a terminal
            {
                const auto& modelTerms = m_tree->GetChildren();

                for (size_t j = 0; j < modelTerms.size(); ++j)
                {
                    predictions[i] += m_coefficients[j+1] * modelTerms[j]->Evaluate();
                }
            }
            else 
            {
                predictions[i] += m_tree->Evaluate();
            }
        }
    }


    void TimeSeriesChromosome::Predict(std::vector<double>& predictionCases, std::vector<double>& terminals, int cutoff) const
    {
        int lag = terminals.size();
        std::vector<double> fitnessCases(predictionCases.begin(), predictionCases.end() - cutoff);
        int totalCases = predictionCases.size() - lag;

        // Iterate over the fitnessCases vector to build the W matrix and Y vector.
        for (int i = 0; i < totalCases; ++i)
        {
            // load up the terminals for this fitness case
            for (size_t j = 0; j < lag; ++j)
            {
                terminals[j] = i+j < fitnessCases.size() ? fitnessCases[i+j] : predictionCases[i+j];
            }

            predictionCases[i+lag] = m_coefficients[0];
            if (m_size != 1) // check that this isn't just a terminal
            {
                const auto& modelTerms = m_tree->GetChildren();

                for (size_t j = 0; j < modelTerms.size(); ++j)
                {
                    predictionCases[i+lag] += m_coefficients[j+1] * modelTerms[j]->Evaluate();
                }
            }
            else 
            {
                predictionCases[i+lag] += m_tree->Evaluate();
            }
        }
    }
}
