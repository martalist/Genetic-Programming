#ifndef ChromosomeFactory_H
#define ChromosomeFactory_H

#include <memory>
#include <string>
#include "ChromosomeType.h"
#include "IChromosome.h"

namespace Model
{
    /**
     * A singleton factory class to create new Chromosomes of a specified type.
     */
    class ChromosomeFactory
    {
    public:
        /**
         * Initialises the factory/singleton
         * @param type The type of Chromosome to be created by the factory
         * @param targetSize The number of nodes in the chromosome tree we'd like. The
         *        number created is not deterministic, so targetSize acts as a minimum.
         * @param allowedFunctions The functions (types) allowed in the S-expressions
         * @param variables A vector of pointers to the terminals
         * @param fitnessCases The training data
         * @param terminals A vector of the terminals of interest
         */
        static void Initialise(ChromosomeType type, int targetSize, 
                const std::vector<FunctionType>& allowedFunctions, 
                const std::vector<double*>& variables,  // TODO: this is probably unecessary
                const std::vector<double>& fitnessCases, 
                std::vector<double>& terminals);

        /**
         * @return a reerence to the singleton instance
         */
        static const ChromosomeFactory& Inst();

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

    private:
        
        /**
         * Constructor
         */
        ChromosomeFactory(ChromosomeType type, int targetSize, const std::vector<FunctionType>& allowedFunctions, 
                    const std::vector<double*>& variables, const std::vector<double>& fitnessCases, 
                    std::vector<double>& terminals);

        // TODO: can these be const?
        const ChromosomeType m_type = ChromosomeType::Normal; //<
        const int m_targetSize;
        const std::vector<FunctionType> m_allowedFunctions;
        const std::vector<double*>& m_variables;
        const std::vector<double>& m_fitnessCases;
        std::vector<double>& m_terminals;

        static std::unique_ptr<ChromosomeFactory> s_instance;
    };
}
#endif
