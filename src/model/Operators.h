#ifndef Operators_H
#define Operators_H

#include <vector>
#include "INode.h"
#include "FunctionFactory.h" // TODO: consider moving FunctionType out of this file

namespace Model
{
    // TODO: I'm not sure this needs to be a class
    class Operators
    {
    public:
        /**
         * Performs mutation on a chromosome
         */
        static void Mutate(INode& chromosome);

        /**
         * Performs crossover on a pair of chromosomes
         */
        static void Crossover(INode& left, INode& right);

        // TODO: pass the vector of FunctionType and Variables here
        static std::unique_ptr<INode> CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions);

        // TODO: maybe doesn't need to be private? And if so, put it in an
        // anonymous namespace
    private:

        /**
         * Gets a random index into a collection of the specified size
         * @param size The size of the collection
         */
        static int RandomIndex(size_t size);
    };

}

#endif
