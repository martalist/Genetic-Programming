#ifndef Program_H
#define Program_H

#include <memory>
#include "PopulationParams.h"
#include "Population.h"
#include "utils/Logger.h"

namespace Model
{
    /**
     * Allows configuration and running of a genetic programming stochastic 
     * search program.
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
        int m_numGenerations = 20; ///< Number of generations to evolve through to find a solution
        Util::Logger<double, double, std::string> m_logger;
    };
}

#endif
