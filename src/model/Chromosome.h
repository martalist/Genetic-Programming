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
        using INodePtr = std::unique_ptr<INode>;

    public:
        /**
         * Constructor - Does no fitness calculations upon construction
         * @param tree The underlying S-expression
         */
        Chromosome(INodePtr tree);

        /**
         * Constructor - Calculates fitness and weighted fitness upon construction.
         */
        Chromosome(INodePtr tree, const std::vector<std::vector<double>>& fitnessCases, 
                std::vector<double>& terminals, double parsimonyCoefficient);

        /**
         * Constructor - Calculates only the weighted fitness upon construction.
         */
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
        double Fitness = std::numeric_limits<double>::max(); ///< raw fitness of the chromosome
        double WeightedFitness = std::numeric_limits<double>::max(); ///< weighted fitness, with penalty for length/size
    };
}

#endif
