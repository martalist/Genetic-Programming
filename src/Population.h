#ifndef Population_H
#define Population_H

#include <memory>
#include <tuple>
#include <vector>
#include "model/INode.h"
#include "PopulationParams.h"
#include "utils/UniformRandomGenerator.h"
#include "utils/Raffle.h"

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
        using INodePtr = std::unique_ptr<INode>;

        /**
        * Represents an individual chromosome (S-expression) in the population,
        * together with it's fitness
        */
        struct Chromosome
        {
            Chromosome(INodePtr tree, double fitness) 
                : Tree(std::move(tree))
                , Fitness(fitness) {}

            INodePtr Tree;
            double Fitness = 0.0;
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
         * Calculate the fitness for all chromosomes in the population
         * @param population The population of Chromosomes to evaulate
         */
        void CalculateFitness(std::vector<Chromosome>& population);

        /**
         * @return the average fitness for the population
         */
        double GetAverageFitness() const;

        /**
         * @return the best fitness value for the population
         */
        double GetBestFitness() const;

        /**
         * @return The best S-expression (by fitness) as a string
         */
        std::string BestAsString() const;

    private:

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
         * Calculate the fitness for one chromosome. Currently uses MAE (mean absolute error)
         * @param chromosome The chromosome to evaluate
         * @return the chromosome fitness as a positive, real number
         */
        double CalculateChromosomeFitness(const INode& chromosome);

        std::vector<Chromosome> m_population; ///< The chromosome population
        PopulationParams m_params; ///< The parameters of the population
        mutable Util::UniformRandomGenerator<float> m_randomProbability; ///< Generates random floats in the range [0,1]
        std::vector<double*> m_allowedTerminals; ///< The set of variables
        std::vector<double> m_terminals; ///< The terminal values to evaluate
        std::vector<std::vector<double>> m_fitnessCases; ///< Training cases
        Util::Raffle<double> m_raffle; ///< Ticketing system used to select parents
    };
}

#endif
