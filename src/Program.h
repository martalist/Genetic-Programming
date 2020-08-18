#ifndef Program_H
#define Program_H

#include <memory>
#include <tuple>
#include <vector>
#include "model/INode.h"

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
        Program(/* CLI args */);

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
         * Select a pair of parents
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

        void TemporaryTesting();

        void CalculatePopulationFitness();
        double CalculateChromosomeFitness(unsigned int index);

        unsigned int m_populationSize = 10;
        std::vector<std::unique_ptr<INode>> m_population;
        std::vector<double> m_fitness;
        int m_minInitialTreeSize = 4;
        unsigned int m_numGenerations = 10;
        double m_crossoverProb = 0.7;
        double m_mutationProb = 0.001;
        std::vector<FunctionType> m_allowedFunctions;
    };
}

#endif
