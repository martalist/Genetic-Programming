#ifndef Population_H
#define Population_H

#include <memory>
#include <tuple>
#include <vector>
#include "model/INode.h"
#include "PopulationParams.h"
#include "utils/UniformRandomGenerator.h"
#include "utils/ISelector.h"

namespace Tests
{
    class PopulationTest;
}

namespace Model
{
    enum class FunctionType;

    /**
     * Represents a population of S-expressions, and facilitates reproduction
     * and evolution.
     */
    class Population
    {
    public:
        friend Tests::PopulationTest;
        using INodePtr = std::unique_ptr<INode>;

        /**
        * Represents an individual chromosome (S-expression) in the population,
        * together with it's fitness
        */
        struct Chromosome
        {
            /**
             * Constructor
             */
            Chromosome(Population& pop, INodePtr tree);
            Chromosome(Population& pop, INodePtr tree, double fitness);

            /**
             * Less-than operator. Used for sorting collections of Chromosomes.
             */
            bool operator<(const Chromosome& rhs) const;

            /**
            * Calculate the fitness for one chromosome. Currently uses MAE (mean absolute error)
            * @param chromosome The chromosome to evaluate
            * @return the chromosome fitness as a positive, real number
            */
            double CalculateFitness(Population& pop) const;

            double CalculateWeightedFitness(const Population& pop) const;

            INodePtr Tree; ///< the S-expression
            int Size; ///< the length (nodes in the tree)
            double Fitness; ///< raw fitness of the chromosome
            double WeightedFitness; ///< weighted fitness, with penalty for length/size
        };

        /**
         * Constructor
         */
        Population(const PopulationParams& params, const std::vector<std::vector<double>>& fitnessCases);
        
        /**
         * Resets the population to a new, randomly created state
         */
        void Reset();

        /**
         * Replace the entire population with it's direct descendants.
         */
        void Evolve();

        /**
         * @return the minimum, first quartile, median, third quartile and maximum fitness.
         * @pre Assumes that the population has already been sorted.
         */
        std::tuple<double, double, double, double, double> GetRangeStatistics() const;

        /**
         * @return the average fitness for the population
         */
        double GetAverageFitness() const;

        /**
         * @return The best S-expression (by fitness) as a string
         */
        std::string BestAsString() const;

    private:

        /**
         * Prepares the selector, such that appropriate parents may be selected
         * Called after a new generation/population has been created.
         */
        void RecalibrateParentSelector();

        /**
         * Sorts the m_population by WeightedFitness, and sorts a pointers to the Chromosomes in the population
         * in order of Fitness.
         */
        void SortPopulation();

        /**
         * Select a pair of parents. The more 'fit' the chromosome, the more
         * likely it will be selected as a parent.
         * @return a pointer to two parents
         */
        std::tuple<Chromosome*, Chromosome*> SelectParents();

        /**
         * Adds two offspring from mum and dad to the nextGeneration
         * @param mum The mother chromosome 
         * @param dad The father chromosome
         * @param nextGeneration The next generation of chromosomes (that will replace current generation)
         */
        void Reproduce(const Chromosome& mum, const Chromosome& dad, std::vector<Chromosome>& nextGeneration);

        /**
         * Deep copy from parents, perform crossover and mutation 
         * @return Two offsprint S-expressions
         */
        std::tuple<INodePtr, INodePtr> GetNewOffspring(const Chromosome& mum, const Chromosome& dad) const;

        /**
         * Updates the parsimony coefficient
         */
        double UpdateParsimonyCoefficient();

        std::vector<Chromosome> m_population; ///< The chromosome population
        std::vector<Chromosome*> m_sortedByFitness; ///< Pointers to the chromosome population, sorted by fitness
        PopulationParams m_params; ///< The parameters of the population
        mutable Util::UniformRandomGenerator<float> m_randomProbability; ///< Generates random floats in the range [0,1]
        std::vector<double*> m_allowedTerminals; ///< The set of variables
        std::unique_ptr<Util::ISelector<double>> m_selector; ///< Ticketing system used to select parents

        std::vector<double> m_terminals; ///< The terminal values to evaluate
        std::vector<std::vector<double>> m_fitnessCases; ///< Training cases
        double m_parsimonyCoefficient = 0.0;
    };
}

#endif
