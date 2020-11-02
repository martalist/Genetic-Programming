#ifndef ChromosomeFactory_H
#define ChromosomeFactory_H

#include <memory>
#include <optional>
#include <string>
#include "ChromosomeType.h"
#include "IChromosome.h"

namespace Model
{
    struct TrainingData;

    /**
     * A singleton factory class to create new Chromosomes of a specified type.
     */
    class ChromosomeFactory
    {
    public:
        /**
         * Constructor
         */
        ChromosomeFactory(ChromosomeType type, int targetSize, 
                const std::vector<FunctionType>& allowedFunctions, 
                const std::vector<double*>& variables, 
                const TrainingData& fitnessCases, 
                std::vector<double>& terminals);

        /**
         * Create a new, random Chromosome
         * @param parsimonyCoefficient The coefficient used to penalise long chromosomes
         */
        std::unique_ptr<IChromosome> CreateRandom(double parsimonyCoefficient) const;

        /**
         * Create a Chromosome
         * @param tree The pre-build INode tree for the Chromosome. Ownership of the tree is transferred
         *        to the new Chromosome.
         * @param parsimonyCoefficient The coefficient used to penalise long chromosomes
         * @return the new Chromosome
         */
        std::unique_ptr<IChromosome> CopyAndEvaluate(std::unique_ptr<INode> tree, double parsimonyCoefficient) const;

        /**
         * Sets the seed for all chromosomes
         */
        void SetSeed(int seed);

    private:

        // TODO: can these be const?
        const ChromosomeType m_type = ChromosomeType::Normal; //<
        const int m_targetSize;
        const std::vector<FunctionType> m_allowedFunctions;
        const std::vector<double*>& m_variables;
        const TrainingData& m_fitnessCases;
        std::vector<double>& m_terminals;

        /**
         * The random number generator for all Chromosomes in the population
         */
        mutable Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>> m_randInt;
    };
}
#endif
