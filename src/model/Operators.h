#ifndef Operators_H
#define Operators_H

#include <vector>
#include "INode.h"
#include "FunctionFactory.h" // TODO: consider moving FunctionType out of this file

namespace Model { namespace Operators
{
    /**
     * Utility function to obtain deterministic results from the operators
     * @param seed The seed to set
     */
    void SetSeed(int seed);
    
    /**
     * Performs standard mutation on a chromosome. 
     * 
     * A gene is randomly selected, and mutated to another valid, random gene from the set of allowed 
     * functions and terminals.
     * @param chromosome The S-expression to mutate
     * @param allowedFunctions The allowed set of functions that may be selected from
     * @param variables The allowed set of terminals that may be selected from
     */
    void Mutate(std::unique_ptr<INode>& chromosome, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables);

    /**
     * Performs hoist mutation on a chromosome. 
     *
     * Selects a gene (subtree) at random from within the chromosome, then randomly selects another node
     * within the subtree. The second gene is hoisted into the position of the first. This type of 
     * mutation is beneficial to reduce the size of chromosomes.
     * @param chromosome The S-expression to mutate.
     */
    void HoistMutate(std::unique_ptr<INode>& chromosome);

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
    std::unique_ptr<INode> CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables);
}}

#endif
