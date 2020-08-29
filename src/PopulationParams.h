#ifndef PopulationParams_h
#define PopulationParams_h

namespace Model
{
    enum class FunctionType;

    struct PopulationParams
    {
        int PopulationSize = 10; ///< Number of chromosomes in the population
        int MinInitialTreeSize = 4; ///< The minimum tree size of newly (randomly) created S-expressions
        double CrossoverProb = 0.7; ///< The probability that two child genes cross over
        double MutationProb = 0.001; ///< The probability that a child gene mutates
        std::vector<FunctionType> AllowedFunctions; ///< The set of functions permitted in chromosomes
        std::vector<double*> AllowedTerminals; ///< The set of variables
    };
}

#endif
