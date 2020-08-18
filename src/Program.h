#ifndef Program_H
#define Program_H

#include <memory>
#include <tuple>
#include <vector>
#include "model/INode.h"
#include "utils/UniformRandomGenerator.h"

namespace Model
{
    enum class FunctionType;

    /**
     * An interface Node of the genetic programming tree/model.
     */
    class Program
    {
    public:
        /**
         * Constructor
         */
        Program(/* TODO: CLI args */);

        /**
         * Runs the genetic program for the specified number of generations
         */
        void Start();

    private:
        using NodePair = std::tuple<std::unique_ptr<INode>, std::unique_ptr<INode>>;

        /**
         * Determines the fitness of each chromosome/program in the 
         * population.
         */
        void EvaluateFitness();

        /**
         * Select a pair of parents. The more 'fit' the chromosome, the more
         * likely it will be selected as a parent.
         */
        std::tuple<INode*, INode*> SelectParents();
        
        /**
         * Replace the entire population with it's direct descendants.
         */
        void ProduceNextGeneration();

        /**
         * Deep copy from parents, perform crossover and mutation
         */
        NodePair Reproduce(const INode& mum, const INode& dad);

        /**
         * TODO: move this to /tests
         */
        void TemporaryTesting();

        /**
         * Calculate the fitness for all chromosomes in the population
         */
        void CalculatePopulationFitness();

        /**
         * Calculate the fitness for one chromosome
         * @param index The index of the chromosome
         */
        double CalculateChromosomeFitness(unsigned int index);

        unsigned int m_populationSize = 10; ///< Number of chromosomes in the population
        std::vector<std::unique_ptr<INode>> m_population; ///< The chromosome population
        std::vector<double> m_fitness; ///< TODO
        int m_minInitialTreeSize = 4;
        unsigned int m_numGenerations = 10; ///< Number of generations to evolve through to find a solution
        double m_crossoverProb = 0.7; ///< The probability that two child genes cross over
        double m_mutationProb = 0.001; ///< The probability that a child gene mutates
        Util::UniformRandomGenerator<float> m_randomProbability; ///< Generates random floats in the range [0,1]
        std::vector<FunctionType> m_allowedFunctions; ///< The set of functions permitted in chromosomes
    };
}

#endif
