#ifndef Program_H
#define Program_H

#include <utility>
#include <vector>

namespace Model
{
    class INode;

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

        // std::vector<INode&> m_population;
        unsigned int m_populationSize = 10;
        unsigned int m_numGenerations = 10;
        double m_crossoverProb = 0.7;
        double m_mutationProb = 0.001;
    };
}

#endif
