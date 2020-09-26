#ifndef PopulationParams_h
#define PopulationParams_h

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #include <experimental/optional>
    #define OPTIONAL std::experimental::optional
#elif __APPLE__
    #include <optional>
    #define OPTIONAL std::optional
#endif

namespace Model
{
    enum class FunctionType;

    /**
     * Parameters that define the population and how it evolves
     */
    struct PopulationParams
    {
        int PopulationSize = 500; ///< Number of chromosomes in the population
        int MinInitialTreeSize = 10; ///< The minimum tree size of newly (randomly) created S-expressions
        double CrossoverProb = 0.7; ///< The probability that two child genes cross over
        double MutationProb = 0.001; ///< The probability that a child gene mutates
        std::vector<FunctionType> AllowedFunctions; ///< The set of functions permitted in chromosomes
        int NumberOfTerminals; ///< The number of terminals
        OPTIONAL<int> Seed;
    };
}

#endif
