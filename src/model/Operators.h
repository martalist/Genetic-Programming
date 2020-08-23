#ifndef Operators_H
#define Operators_H

#include <vector>
#include "INode.h"
#include "FunctionFactory.h" // TODO: consider moving FunctionType out of this file

namespace Model { namespace Operators
{
    /**
     * Performs mutation on a chromosome
     * @param chromosome The S-expression to mutate
     * @param allowedFunctions The allowed set of functions that may be selected from
     * @param variables The allowed set of terminals that may be selected from
     */
    void Mutate(std::unique_ptr<INode>& chromosome, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*> variables);

    /**
     * Performs crossover on a pair of chromosomes
     * @param left The first S-expression/chromosome
     * @param right The second S-expression/chromosome
     */
    void Crossover(std::unique_ptr<INode>& left, std::unique_ptr<INode>& right);

    /**
     * Creates a new, random chromosome
     * @param targetSize The number of nodes in the chromosome tree we'd like. The
     * number created is not deterministic, so targetSize acts as a minimum.
     * @param allowedFunctions The set of functions allowed in the chromosome tree.
     * @param variables The allowed set of terminals that may be selected from
     * @return the root of the new chromosome
     */
    std::unique_ptr<INode> CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*> variables);
}}

#endif
