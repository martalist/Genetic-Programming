#ifndef PopulationParams_h
#define PopulationParams_h

#include <experimental/optional>

namespace Model
{
    enum class FunctionType;

    /**
     * Parameters that define the population and how it evolves
     */
    struct PopulationParams
    {
        int PopulationSize = 10; ///< Number of chromosomes in the population
        int MinInitialTreeSize = 4; ///< The minimum tree size of newly (randomly) created S-expressions
        double CrossoverProb = 0.7; ///< The probability that two child genes cross over
        double MutationProb = 0.001; ///< The probability that a child gene mutates
        std::vector<FunctionType> AllowedFunctions; ///< The set of functions permitted in chromosomes
        int NumberOfTerminals; ///< The number of terminals
        std::experimental::optional<int> Seed;
    };
}

#endif
