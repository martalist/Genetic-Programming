#ifndef Chromosome_H
#define Chromosome_H

#include <memory>
#include <vector>
#include "IChromosome.h"

namespace Tests
{
    class PopulationTest;
}

namespace Model
{
    /**
     * Represents an individual chromosome (S-expression) in the population,
     * together with it's fitness
     */
    class Chromosome : public IChromosome
    {
    public:
        friend Tests::PopulationTest;

        /**
         * Constructor - Creates a random Chromosome
         */
        Chromosome(int targetSize, 
                    const std::vector<FunctionType>& allowedFunctions, 
                    const std::vector<double*>& variables,
                    const std::vector<double>& fitnessCases, 
                    std::vector<double>& terminals, 
                    double parsimonyCoefficient);

        /**
         * Constructor - Does no fitness calculations upon construction
         * @param tree The underlying S-expression
         */
        Chromosome(IChromosome::INodePtr tree);

        /**
         * Copy Constructor
         */
        Chromosome(const Chromosome& other);

        /**
         * Constructor - Calculates fitness and weighted fitness upon construction.
         */
        Chromosome(IChromosome::INodePtr tree, const std::vector<double>& fitnessCases, 
                std::vector<double>& terminals, double parsimonyCoefficient);

        /**
         * Constructor - Calculates only the weighted fitness upon construction.
         */
        Chromosome(IChromosome::INodePtr& tree, double fitness, double parsimonyCoefficient);

        /**
         * Less-than operator. Used for sorting collections of Chromosomes.
         */
        bool operator<(const IChromosome& right) const override;

        /**
         * @see IChromosome::Size
         */
        int Size() const override;

        /**
         * @see IChromosome::Fitness
         */
        double Fitness() const override;

        /**
         * @see IChromosome::Clone
         */
        std::unique_ptr<IChromosome> Clone() const override;

        /**
         * @see IChromosome::Mutate
         */
        void Mutate(const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables) override;

        /**
         * @see IChromosome::HoistMutate
         */
        void HoistMutate() override;

        /**
         * @see IChromosome::Crossover
         */
        void Crossover(IChromosome& right) override;

        /**
         * @see IChromosome::GetTree
         */
        IChromosome::INodePtr& GetTree() override;
        const IChromosome::INodePtr& GetTree() const override;

        /**
         * Creates a new, random chromosome
         * @param targetSize The number of nodes in the chromosome tree we'd like. The
         * number created is not deterministic, so targetSize acts as a minimum.
         * @param allowedFunctions The set of functions allowed in the chromosome tree.
         * @param variables The allowed set of terminals that may be selected from
         * @return the root of the new chromosome
         */
        static std::unique_ptr<INode> CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables);

    private:

        /**
         * Calculate the fitness for one chromosome. Currently uses MAE (mean absolute error)
         * @param chromosome The chromosome to evaluate
         * @return the chromosome fitness as a positive, real number
         */
        double CalculateFitness(const std::vector<double>& fitnessCases, std::vector<double>& terminals) const override;

        double CalculateWeightedFitness(double parsimonyCoefficient) const override;

        /**
         * @see IChromosome::SetSize
         */
        void SetSize(int size) override;

        IChromosome::INodePtr m_tree; ///< the S-expression
        int m_size; ///< the length (nodes in the tree)
        double m_fitness = std::numeric_limits<double>::max(); ///< raw fitness of the chromosome
        double m_weightedFitness = std::numeric_limits<double>::max(); ///< weighted fitness, with penalty for length/size
    };
}

#endif
