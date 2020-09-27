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
        std::vector<std::vector<double>> FitnessCases ///< Training cases
        {
            { 3.0, 5.0, 5.830952 }, // For now, hard-coded. XML file should specify a CSV filename to load these in
            { 8.0, 14.0, 16.124515 },
            { 18.0, 2.0, 18.11077 },
            { 32.0, 11.0, 33.837849 },
            { 4.0, 3.0, 5.0 },
            { 12.0, 10.0, 15.620499 },
            { 21.0, 6.0, 21.84033 },
            { 7.0, 4.0, 8.062258 },
            { 16.0, 24.0, 28.84441 },
            { 2.0, 9.0, 9.219545 }
        };
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

        static Config s_config; ///< the loaded/default config
    };
}
#endif 
