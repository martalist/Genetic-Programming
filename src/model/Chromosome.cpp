#include "Chromosome.h"

#include <algorithm>
#include <cmath>

namespace Model
{
    Chromosome::Chromosome(Chromosome::INodePtr tree)
        : Tree(std::move(tree)) 
        , Size(Tree->Size())
    {
    }

    Chromosome::Chromosome(Chromosome::INodePtr tree, const std::vector<std::vector<double>>& fitnessCases, 
            std::vector<double>& terminals, double parsimonyCoefficient)
        : Tree(std::move(tree)) 
        , Size(Tree->Size())
        , Fitness(CalculateFitness(fitnessCases, terminals))
        , WeightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    } 

    Chromosome::Chromosome(Chromosome::INodePtr tree, double fitness, double parsimonyCoefficient)
        : Tree(std::move(tree)) 
        , Size(Tree->Size())
        , Fitness(fitness)
        , WeightedFitness(CalculateWeightedFitness(parsimonyCoefficient))
    {
    } 

    bool Chromosome::operator<(const Chromosome& rhs) const
    {
        return WeightedFitness < rhs.WeightedFitness;
    }

    double Chromosome::CalculateFitness(const std::vector<std::vector<double>>& fitnessCases, std::vector<double>& terminals) const
    {
        double sumOfErrors = 0.0;
        for (const auto& fCase : fitnessCases) // FitnessCases are the training set
        {
            // load up the terminals for this fitness case
            std::copy(fCase.begin(), fCase.end()-1, terminals.begin());

            // calculate the fitness (absolute error) for this fitness case
            auto returnVal = Tree->Evaluate();

            // add to the tally
            sumOfErrors += std::abs(returnVal - fCase.back());
        }
        return sumOfErrors / fitnessCases.size(); // mean absolute error
    }

    double Chromosome::CalculateWeightedFitness(double parsimonyCoefficient) const
    {
        return Fitness + parsimonyCoefficient * Size;
    }
}
