#ifndef ChromosomeUtil_H
#define ChromosomeUtil_H

#include <vector>
#include "../utils/UniformRandomGenerator.h"

namespace Model
{
    class INode;

    namespace ChromosomeUtil
    {
        /**
         * A random integer generator for reuse in the methods below
         */
        Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>>& RandInt();

        /**
         * Sets the seed of the random number generator.
         */
        void SetSeed(int seed);

        /**
        * @param gene The S-expression gene to inspect
        * @return true if the gene is a Terminal (not a Function)
        */
        bool IsTerminal(const std::unique_ptr<INode>& gene);

        /**
         * Gets a random index into a collection of the specified size
         * @param size The size of the collection
         */
        int RandomIndex(size_t size);

        /**
         * Adds terminals to a function to ensure it has a valid number of children.
         * @pre This should only be called on Function objects.
         * @param func The function to fill.
         */
        void FillFunction(Model::INode* func, const std::vector<double*>& variables);
    }
}
#endif
