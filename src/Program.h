#ifndef Program_H
#define Program_H

#include <memory>
#include <utility>
#include <vector>
#include "utils/UniformRandomGenerator.h"

namespace Model
{
    class INode;
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

        void Start();

    private:
        using NodePair = std::pair<INode&, INode&>;

        /**
         * Determines the fitness of each chromosome/program in the 
         * population.
         */
        void EvaluateFitness();

        /**
         * Select a pair of parents
         */
        NodePair SelectParents();
        
        /**
         * Deep copy from parents, perform crossover and mutation
         */
        NodePair Reproduce(const INode& mum, const INode& dad);

        void TemporaryTesting();

        std::unique_ptr<INode> CreateRandomChromosome() const;

        int RandomIndex(size_t size) const;

        // std::vector<std::unique_ptr<INode>> m_population;
        int m_minInitialTreeSize = 4;
        unsigned int m_populationSize = 10;
        unsigned int m_numGenerations = 10;
        double m_crossoverProb = 0.7;
        double m_mutationProb = 0.001;
        std::vector<FunctionType> m_allowedFunctions;

        static Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>> s_randInt;
    };
}

#endif
