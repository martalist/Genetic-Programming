#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "PopulationParams.h"

namespace Model
{
    /**
     * All configuration required to run Integenetics.
     * Made available through XML to allow changing parameters without re-compilation.
     */
    struct Config
    {
        int Iterations = 1; ///< Number of times to run the experiment
        int NumGenerations = 20; ///< Number of generations to evolve through to find a solution
        PopulationParams Params{}; ///< Parameters for the population
        std::vector<std::vector<double>> FitnessCases; ///< Training cases
    };

    /**
     * Provides static methods to load configuration from user-specified XML.
     */
    class ConfigParser
    {
    public:
        /**
         * Loads configuration for Integenetics from file
         * @param filename The XML from which to load config
         * @return Configuration for the program to execute
         */
        static Config Load(const std::string& filename);

    private:
        /**
         * Prints the config to stdout
         */
        static void PrintConfig();

        /**
         * Loads the fitness cases from the CSV file specified
         * @param filename The CSV file name to load
         * @return the number of Terminals in the CSV's fitness cases
         */
        static int LoadFitnessCases(const std::string& filename);

        static Config s_config; ///< the loaded/default config
    };
}
#endif 
