#ifndef PopulationParams_h
#define PopulationParams_h

#include <optional>
#include <vector>
#include "model/ChromosomeType.h"

namespace Model
{
    enum class FunctionType;

    /**
     * Parameters that define the population and how it evolves
     */
    struct PopulationParams
    {
        ChromosomeType Type = ChromosomeType::Normal; ///< The type of Chromosome (and therefore underlying Program Type)
        int PopulationSize = 1000; ///< Number of chromosomes in the population
        int MinInitialTreeSize = 10; ///< The minimum tree size of newly (randomly) created S-expressions
        double CrossoverProb = 0.7; ///< The probability that two child genes cross over
        double MutationProb = 0.001; ///< The probability that a child gene mutates
        double HoistMutationProb = 0.001; ///< The probability that hoist mutation occurs when reproducing
        std::vector<FunctionType> AllowedFunctions; ///< The set of functions permitted in chromosomes

        /**
         * The number of terminals. These are in independent variables in Normal 
         * mode, and autoregressive terms in TimeSeries mode (e.g. 8 --> [X_{t-1}, ... , X_{t-8}]).
         */
        int NumberOfTerminals = 1;
        std::optional<int> Seed; ///< The seed, if set, to yield deterministic results
        
        /**
         * The number of pairs of offspring to produce from each set of parents. i.e. Setting this to 3 will 
         * mean 6 children are created per pair of parents, from which the best 2 will be placed into the next
         * generation. (i.e. only two survive beyond birth).
         */
        int TwinsPerMatingPair = 3;

        /**
         * The proportion of the population to carry into the next generation, in order of fitness.
         * Used to ensure the "best" fitness never gets worse from one generation to the next.
         */
        double CarryOverProportion = 0.0;
    };

    /**
     * All configuration required to run Integenetics.
     * Made available through XML to allow changing parameters without re-compilation.
     */
    struct Config
    {
        int Iterations = 1; ///< Number of times to run the experiment
        int NumGenerations = 20; ///< Number of generations to evolve through to find a solution
        std::optional<double> StoppingCriteria;
        PopulationParams Params{}; ///< Parameters for the population
        std::vector<double> FitnessCases; ///< Training cases
    };
}

#endif
