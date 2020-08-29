#ifndef Population_H
#define Population_H

#include <memory>
#include <tuple>
#include <vector>
#include "model/INode.h"
#include "utils/UniformRandomGenerator.h"
#include "utils/Raffle.h"

namespace Model
{
    enum class FunctionType;
    struct PopulationParams;

    /**
     * Represents a population of S-expressions, and facilitates reproduction
     * and evolution.
     */
    class Population
    {
    public:
        /**
         * Constructor
         */
        Population(const PopulationParams& params);
        
        /**
         * Replace the entire population with it's direct descendants.
         */
        void Evolve();

        /**
         * Calculate the fitness for all chromosomes in the population
         */
        void CalculateFitness(const std::vector<std::vector<double>>& fitnessCases);

        /**
         * @return the average fitness for the population
         */
        double GetAverageFitness() const;

        /**
         * @return the best fitness value for the population
         */
        double GetBestFitness() const;

        std::string BestAsString() const;

    private:
        using NodePair = std::tuple<std::unique_ptr<INode>, std::unique_ptr<INode>>;

        /**
         * Select a pair of parents. The more 'fit' the chromosome, the more
         * likely it will be selected as a parent.
         * @return TODO
         */
        std::tuple<INode*, INode*> SelectParents();

        /**
         * Deep copy from parents, perform crossover and mutation
         * @param TODO
         */
        NodePair Reproduce(const INode& mum, const INode& dad);

        /**
         * Calculate the fitness for one chromosome
         * @param index The index of the chromosome
         * @param fitnessCases TODO
         * @return TODO
         */
        double CalculateChromosomeFitness(unsigned int index, const std::vector<std::vector<double>>& fitnessCases);

        std::vector<std::unique_ptr<INode>> m_population; ///< The chromosome population
        std::vector<double> m_fitness; ///< The finess of chromosomes, in order of m_population
        const double m_crossoverProb = 0.7; ///< The probability of genetic crossover when breeding
        const double m_mutationProb = 0.001; ///< The probability of gene mutation when breeding
        Util::UniformRandomGenerator<float> m_randomProbability; ///< Generates random floats in the range [0,1]
        std::vector<FunctionType> m_allowedFunctions; ///< The set of functions permitted in chromosomes
        std::vector<double*> m_allowedTerminals; ///< The set of variables
        std::vector<double> m_terminals; ///< The terminal values to evaluate
        Util::Raffle<double> m_raffle;
    };
}

#endif
