#ifndef Chromosome_H
#define Chromosome_H

#include <memory>
#include <vector>
#include "INode.h"

namespace Model
{
    enum class FunctionType;

    /**
     * Represents an individual chromosome (S-expression) in the population,
     * together with it's fitness
     */
    class Chromosome
    {
    public:
        using INodePtr = std::unique_ptr<INode>;

        /**
         * Constructor
         */
        Chromosome(INodePtr tree, const std::vector<std::vector<double>>& fitnessCases, 
                std::vector<double>& terminals, double parsimonyCoefficient);
        Chromosome(INodePtr tree, double fitness, double parsimonyCoefficient);

        /**
         * Less-than operator. Used for sorting collections of Chromosomes.
         */
        bool operator<(const Chromosome& rhs) const;

        /**
         * Calculate the fitness for one chromosome. Currently uses MAE (mean absolute error)
         * @param chromosome The chromosome to evaluate
         * @return the chromosome fitness as a positive, real number
         */
        double CalculateFitness(const std::vector<std::vector<double>>& fitnessCases, std::vector<double>& terminals) const;

        double CalculateWeightedFitness(double parsimonyCoefficient) const;

        INodePtr Tree; ///< the S-expression
        int Size; ///< the length (nodes in the tree)
        double Fitness; ///< raw fitness of the chromosome
        double WeightedFitness; ///< weighted fitness, with penalty for length/size
    };
}

#endif
