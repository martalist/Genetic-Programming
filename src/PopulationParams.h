#ifndef PopulationParams_h
#define PopulationParams_h

#include <optional>
#include <vector>

namespace Model
{
    enum class FunctionType;

    /**
     * Parameters that define the population and how it evolves
     */
    struct PopulationParams
    {
        int PopulationSize = 5000; ///< Number of chromosomes in the population
        int MinInitialTreeSize = 10; ///< The minimum tree size of newly (randomly) created S-expressions
        double CrossoverProb = 0.7; ///< The probability that two child genes cross over
        double MutationProb = 0.001; ///< The probability that a child gene mutates
        std::vector<FunctionType> AllowedFunctions; ///< The set of functions permitted in chromosomes
        int NumberOfTerminals; ///< The number of terminals
        std::optional<int> Seed;
        bool AlwaysReplaceParents = false;
    };
}

#endif
