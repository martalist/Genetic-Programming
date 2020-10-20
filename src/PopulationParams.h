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
        int PopulationSize = 5000; ///< Number of chromosomes in the population
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
        bool AlwaysReplaceParents = false; ///< If true, the parents are always replaced by the 1st two children birthed.
    };
}

#endif
