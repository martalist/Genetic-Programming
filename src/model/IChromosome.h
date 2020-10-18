#ifndef IChromosome_H
#define IChromosome_H

#include <vector>
#include "INode.h"

namespace Model
{
    enum class FunctionType;

    /**
     * Represents an individual chromosome (S-expression) in the population,
     * together with it's fitness
     */
    class IChromosome
    {
    protected:
        using INodePtr = std::unique_ptr<INode>;

    public:
        /**
         * Constructor - Creates a new, random Chromosom
         */
        // IChromosome(int targetSize);

        /**
         * Constructor - Does no fitness calculations upon construction
         * @param tree The underlying S-expression
         */
        // IChromosome(INodePtr tree);

        /**
         * Constructor - Calculates fitness and weighted fitness upon construction.
         */
        // IChromosome(INodePtr tree, const std::vector<std::vector<double>>& fitnessCases, 
                // std::vector<double>& terminals, double parsimonyCoefficient);

        /**
         * Constructor - Calculates only the weighted fitness upon construction.
         */
        // IChromosome(INodePtr tree, double fitness, double parsimonyCoefficient);

        virtual ~IChromosome() = default;

        /**
         * Less-than operator. Used for sorting collections of Chromosomes.
         */
        virtual bool operator<(const IChromosome& rhs) const = 0;

        /**
         * @return the size/length of the Chromosome
         */
        virtual int Size() const = 0;

        /**
         * @return the fitness of the Chromosome
         */
        virtual double Fitness() const = 0;

        /**
         * Performs standard mutation on a chromosome. 
         * 
         * A gene is randomly selected, and mutated to another valid, random gene from the set of allowed 
         * functions and terminals.
         * @param allowedFunctions The allowed set of functions that may be selected from
         * @param variables The allowed set of terminals that may be selected from
         */
        virtual void Mutate(const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables) = 0;

        /**
         * Performs hoist mutation on a chromosome. 
         *
         * Selects a gene (subtree) at random from within the chromosome, then randomly selects another node
         * within the subtree. The second gene is hoisted into the position of the first. This type of 
         * mutation is beneficial to reduce the size of chromosomes.
         * @param chromosome The S-expression to mutate.
         */
        virtual void HoistMutate() = 0;

        /**
         * Performs crossover on a pair of chromosomes
         * @param left The first S-expression/chromosome
         * @param right The second S-expression/chromosome
         */
        virtual void Crossover(IChromosome& right) = 0;

        /**
         * @return a reference to the tree representation of the Chromosome
         */
        virtual INodePtr& GetTree() = 0;
        virtual const INodePtr& GetTree() const = 0;

        static void SetSeed(int seed);

    protected:
        /**
         * Calculate the fitness for one chromosome.
         * @param chromosome The chromosome to evaluate
         * @return the chromosome fitness as a positive, real number
         */
        virtual double CalculateFitness(const std::vector<std::vector<double>>& fitnessCases, std::vector<double>& terminals) const = 0;

        /**
         * Calculate the weighted fitness of the chromosome, where longer chromosomes are penalized.
         * @param parsimonyCoefficient The coefficient that is multiplied by the Chromosome size.
         */
        virtual double CalculateWeightedFitness(double parsimonyCoefficient) const = 0;

        /**
         * Set the cached size of the Chromosome
         */
        virtual void SetSize(int size) = 0;
    };
}

#endif
