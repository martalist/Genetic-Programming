#ifndef TimeSeriesChromosome_H
#define TimeSeriesChromosome_H

#include <memory>
#include <vector>
#include <Eigen/Dense>
#include "IChromosome.h"

namespace Model
{
    /**
     * Represents an individual chromosome (S-expression) in the population,
     * together with it's fitness.
     * In this case the S-expression is a non-linear, autoregressive (recursive) function
     * for a single time series. Coefficients of the base terms are calculated by least-squares
     * estimation.
     */
    class TimeSeriesChromosome : public IChromosome
    {
    public:

        /**
         * Constructor - Creates a random TimeSeriesChromosome
         */
        TimeSeriesChromosome(int targetSize, 
                    const std::vector<FunctionType>& allowedFunctions, 
                    const std::vector<double*>& variables,
                    const TrainingData& fitnessCases, 
                    std::vector<double>& terminals, 
                    double parsimonyCoefficient,
                    Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>>& rand,
                    FunctionFactory& funcFactory);

        /**
         * Copy Constructor
         */
        TimeSeriesChromosome(const TimeSeriesChromosome& other);

        /**
         * Constructor - Calculates fitness and weighted fitness upon construction.
         */
        TimeSeriesChromosome(IChromosome::INodePtr tree, const TrainingData& fitnessCases, 
                std::vector<double>& terminals, double parsimonyCoefficient,
                Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>>& rand,
                FunctionFactory& funcFactory);

        /**
         * Less-than operator. Used for sorting collections of Chromosomes.
         */
        bool operator<(const IChromosome& right) const override;

        /**
         * @see IChromosome::Size
         */
        int Size() const override;

        /**
         * @see IChromosome::Fitness
         */
        double Fitness() const override;

        /**
         * @see IChromosome::Clone
         */
        std::unique_ptr<IChromosome> Clone() const override;

        /**
         * @see IChromosome::Mutate
         */
        void Mutate(const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables) override;

        /**
         * @see IChromosome::HoistMutate
         */
        void HoistMutate() override;

        /**
         * @see IChromosome::Crossover
         */
        void Crossover(IChromosome& right) override;

        /**
         * @see IChromosome::GetTree
         */
        IChromosome::INodePtr& GetTree() override;
        const IChromosome::INodePtr& GetTree() const override;

        /**
         * @see IChromosome::ToString
         */
        std::string ToString() const override;

        /**
         * @see IChromosome::Forecast
         */
        void Forecast(const TrainingData& fitnessCases, std::vector<double>& terminals, double* predictions, int length) const override;

        /**
         * @see IChromosome::Predict
         */
        void Predict(std::vector<double>& predictionCases, std::vector<double>& terminals, int cutoff = 0) const override;

    private:
        /**
         * Creates a new, random chromosome
         * @param targetSize The number of nodes in the chromosome tree we'd like. The
         * number created is not deterministic, so targetSize acts as a minimum.
         * @param allowedFunctions The set of functions allowed in the chromosome tree.
         * @param variables The allowed set of terminals that may be selected from
         * @return the root of the new chromosome
         */
        std::unique_ptr<INode> CreateRandomChromosome(int targetSize, const std::vector<FunctionType>& allowedFunctions, const std::vector<double*>& variables);

        /**
         * Calculate the fitness for one chromosome. Currently uses MAE (mean absolute error)
         * @param chromosome The chromosome to evaluate
         * @return the chromosome fitness as a positive, real number
         */
        double CalculateFitness(const TrainingData& fitnessCases, std::vector<double>& terminals) override;

        double CalculateWeightedFitness(double parsimonyCoefficient) const override;

        /**
         * @see IChromosome::SetSize
         */
        void SetSize() override;

        IChromosome::INodePtr m_tree; ///< the S-expression
        Eigen::VectorXd m_coefficients; ///< Coefficients of the terms in the autoregressive model
        int m_size; ///< the length (nodes in the tree)
        double m_fitness = std::numeric_limits<double>::max(); ///< raw fitness of the chromosome.
        double m_weightedFitness = std::numeric_limits<double>::max(); ///< weighted fitness, with penalty for length/size
    };
}

#endif
