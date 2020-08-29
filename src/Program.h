#ifndef Program_H
#define Program_H

#include <memory>
#include <tuple>
#include <vector>
#include "PopulationParams.h"
#include "Population.h"
#include "model/INode.h"
#include "utils/UniformRandomGenerator.h"
#include "utils/Raffle.h"

namespace Model
{
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

        PopulationParams m_params{}; ///< parameters for the population
        std::unique_ptr<Population> m_population; ///< The chromosome population
        int m_numGenerations = 10; ///< Number of generations to evolve through to find a solution
    };
}

#endif
