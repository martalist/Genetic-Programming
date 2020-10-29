#ifndef IChromosome_H
#define IChromosome_H

#include <vector>
#include "INode.h"
#include "../utils/UniformRandomGenerator.h"

namespace Model
{
    enum class FunctionType;
    struct TrainingData;

    /**
     * Represents an individual chromosome (S-expression) in the population,
     * together with it's fitness
     */
    class IChromosome
    {
    protected:
        using INodePtr = std::unique_ptr<INode>;

    public:
        IChromosome() = default;
        virtual ~IChromosome() = default;

        /**
         * Less-than operator. Used for sorting collections of Chromosomes.
         */
        virtual bool operator<(const IChromosome& rhs) const = 0;

        /**
         * @return the size/length of the Chromosome
         */
        virtual int Size() const = 0;

        /**
         * @return the fitness of the Chromosome
         */
        virtual double Fitness() const = 0;

        /**
         * @return a clone of the current Chromosome
         */
        virtual std::unique_ptr<IChromosome> Clone() const = 0;

        /**
         * Performs standard mutation on a chromosome. 
         * 
         * A gene is randomly selected, and mutated to another valid, random gene from the set of allowed 
         * functions and terminals.
         * @param allowedFunctions The allowed set of functions that may be selected from
         * @param variables The allowed set of terminals that may be selected from
         */
        virtual void Mutate(const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables) = 0;

        /**
         * Performs hoist mutation on a chromosome. 
         *
         * Selects a gene (subtree) at random from within the chromosome, then randomly selects another node
         * within the subtree. The second gene is hoisted into the position of the first. This type of 
         * mutation is beneficial to reduce the size of chromosomes.
         * @param chromosome The S-expression to mutate.
         */
        virtual void HoistMutate() = 0;

        /**
         * Performs crossover on a pair of chromosomes
         * @param left The first S-expression/chromosome
         * @param right The second S-expression/chromosome
         */
        virtual void Crossover(IChromosome& right) = 0;

        /**
         * @return a reference to the tree representation of the Chromosome
         */
        virtual INodePtr& GetTree() = 0;
        virtual const INodePtr& GetTree() const = 0;

        /**
         * @return the string representation of the Chromosome
         */
        virtual std::string ToString() const = 0;

        /**
         * Allows for prediction of new values.
         * @param fitnessCases The original training data.
         * @param terminals A reference to the Terminals pointed to by each Chromosome (for evaluation).
         * @param prediction A double array that predictions should be written to.
         * @param length The length of the prediction array.
         */
        virtual void Forecast(const TrainingData& fitnessCases, std::vector<double>& terminals, double* predictions, int length) const {}

        /**
         * Returns the result of evaluating the best-fitting Chromesome for a set of prediction cases.
         * @param predictionCases The data to evaluate against
         * @param terminals A reference to the Terminals pointed to by each Chromosome (for evaluation).
         * @param cutoff Used for TimeSeries data to specify where known values end. For example, cutoff=128
         * indicates there are 128 know values, and we want to predict (predictionCases-cutoff) steps ahead.
         */
        virtual void Predict(std::vector<double>& predictionCases, std::vector<double>& terminals, int cutoff = 0) const {}

    protected:
        /**
         * Calculate the fitness for one chromosome.
         * @param chromosome The chromosome to evaluate
         * @return the chromosome fitness as a positive, real number
         */
        virtual double CalculateFitness(const TrainingData& fitnessCases, std::vector<double>& terminals) = 0;

        /**
         * Calculate the weighted fitness of the chromosome, where longer chromosomes are penalized.
         * @param parsimonyCoefficient The coefficient that is multiplied by the Chromosome size.
         */
        virtual double CalculateWeightedFitness(double parsimonyCoefficient) const = 0;

        /**
         * Set the cached size of the Chromosome
         */
        virtual void SetSize() = 0;
    };
}

#endif
