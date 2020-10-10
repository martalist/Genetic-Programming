#ifndef Population_H
#define Population_H

#include <memory>
#include <tuple>
#include <vector>
#include "model/INode.h"
#include "PopulationParams.h"
#include "utils/UniformRandomGenerator.h"
#include "utils/ISelector.h"

namespace Model
{
    enum class FunctionType;

    /**
     * Represents a population of S-expressions, and facilitates reproduction
     * and evolution.
     */
    class Population
    {
    public:
        using INodePtr = std::unique_ptr<INode>;

        /**
        * Represents an individual chromosome (S-expression) in the population,
        * together with it's fitness
        */
        struct Chromosome
        {
            /**
             * Constructor
             */
            Chromosome(INodePtr tree);
            Chromosome(INodePtr tree, double fitness);

            /**
             * Less-than operator. Used for sorting collections of Chromosomes.
             */
            bool operator<(const Chromosome& rhs) const;

            /**
            * Calculate the fitness for one chromosome. Currently uses MAE (mean absolute error)
            * @param chromosome The chromosome to evaluate
            * @return the chromosome fitness as a positive, real number
            */
            double CalculateFitness();

            INodePtr Tree;
            double Fitness;
        };

        /**
         * Constructor
         */
        Population(const PopulationParams& params, const std::vector<std::vector<double>>& fitnessCases);
        
        /**
         * Resets the population to a new, randomly created state
         */
        void Reset();

        /**
         * Replace the entire population with it's direct descendants.
         */
        void Evolve();

        /**
         * Prepares the selector, such that appropriate parents may be selected
         * Called after a new generation/population has been created.
         */
        void RecalibrateParentSelector();

        /**
         * @return the average fitness for the population
         */
        double GetAverageFitness() const;

        /**
         * @return the best fitness value for the population
         */
        double GetBestFitness() const;

        /**
         * @return The best S-expression (by fitness) as a string
         */
        std::string BestAsString() const;

    private:

        /**
         * Select a pair of parents. The more 'fit' the chromosome, the more
         * likely it will be selected as a parent.
         * @return a pointer to two parents
         */
        std::tuple<Chromosome*, Chromosome*> SelectParents();

        /**
         * Adds two offspring from mum and dad to the nextGeneration
         * @param mum The mother chromosome 
         * @param dad The father chromosome
         * @param nextGeneration The next generation of chromosomes (that will replace current generation)
         */
        void Reproduce(const Chromosome& mum, const Chromosome& dad, std::vector<Chromosome>& nextGeneration);

        /**
         * Deep copy from parents, perform crossover and mutation 
         * @return Two offsprint S-expressions
         */
        std::tuple<INodePtr, INodePtr> GetNewOffspring(const Chromosome& mum, const Chromosome& dad) const;

        std::vector<Chromosome> m_population; ///< The chromosome population
        PopulationParams m_params; ///< The parameters of the population
        mutable Util::UniformRandomGenerator<float> m_randomProbability; ///< Generates random floats in the range [0,1]
        std::vector<double*> m_allowedTerminals; ///< The set of variables
        std::unique_ptr<Util::ISelector<double>> m_selector; ///< Ticketing system used to select parents

        // TODO: these should probably be moved out to Program (from Population)
        static std::vector<double> s_terminals; ///< The terminal values to evaluate
        static std::vector<std::vector<double>> s_fitnessCases; ///< Training cases
    };
}

#endif
