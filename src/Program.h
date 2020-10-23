#ifndef Program_H
#define Program_H

#include <memory>
#include <optional>
#include <vector>
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
         * Constructor (skips fetching config from XML)
         * @param config - The config for the program 
         */
        Program(const Config& config);

        /**
         * Runs the genetic program for the specified number of generations
         */
        void Start(bool logResults = true);

        /**
         * Provides forecast data based on the best fit Program/Chromosome.
         * @param predictions The double array to write forecasts into.
         * @param length The length of the data array (and therefore the number of predictions to make)
         * @return The standard error of the fit chromosome.
         * @pre Start must be called prior to Forecast.
         * @pre The ChromosomeType for this Program must be TimeSeries.
         */
        double Predict(double* predictions, int length);

    private:
        PopulationParams m_params{}; ///< parameters for the population
        std::unique_ptr<Population> m_population; ///< The chromosome population
        int m_numGenerations = 20; ///< Number of generations to evolve through to find a solution
        int m_iterations = 1; ///< Number of times to run the experiment
        std::optional<double> m_stoppingCriteria;
        Util::Logger<double, double, double, double, double, std::string> m_logger{""};
    };
}

#endif
