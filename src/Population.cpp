#include "Population.h"

#include <iostream>
#include "PopulationParams.h"
#include "model/FunctionFactory.h"
#include "model/Terminal.h"
#include "model/Operators.h"

namespace Model
{
    Population::Population(const PopulationParams& params)
        : m_crossoverProb(params.CrossoverProb)
        , m_mutationProb(params.MutationProb)
        , m_randomProbability(.0, 1.)
        , m_allowedFunctions(params.AllowedFunctions)
        , m_allowedTerminals(params.AllowedTerminals)
    {
        // generate m_params.PopulationSize chromosomes
        for (auto i = 0; i < params.PopulationSize; ++i)
        {
            auto itsABoy = Operators::CreateRandomChromosome(
                    params.MinInitialTreeSize, 
                    m_allowedFunctions, 
                    m_allowedTerminals);
            m_population.push_back(std::move(itsABoy));
        }
    }

    void Population::Evolve()
    {
        std::vector<std::unique_ptr<INode>> newPopulation;
        while (newPopulation.size() < m_population.size())
        {
            // select a breeding pair
            auto parents = SelectParents();
            auto& mum = std::get<0>(parents);
            auto& dad = std::get<1>(parents);

            // perform a deep copy, crossover & mutation
            auto children = Reproduce(*mum, *dad);
            
            // add to new population
            newPopulation.push_back(std::move(std::get<0>(children)));
            newPopulation.push_back(std::move(std::get<1>(children)));
        }
    }

    Population::NodePair Population::Reproduce(const INode& mum, const INode& dad)
    {
        // Deep copy mum & dad
        auto son      = FunctionFactory::Copy(dad); // TODO
        auto daughter = FunctionFactory::Copy(mum);
        
        // should we crossover? 
        if (m_randomProbability.Get() <= m_crossoverProb)
        {
            Operators::Crossover(son, daughter);
        }

        // should we mutate?
        if (m_randomProbability.Get() <= m_mutationProb)
        {
            Operators::Mutate(son, m_allowedFunctions, m_allowedTerminals);
        }
        if (m_randomProbability.Get() <= m_mutationProb)
        {
            Operators::Mutate(daughter, m_allowedFunctions, m_allowedTerminals);
        }

        return std::make_tuple(std::move(son), std::move(daughter));
    }
    
    void Population::CalculateFitness(const std::vector<std::vector<double>>& fitnessCases)
    {
        m_raffle.Reset(); // get rid of the previous generation's tickets
        for (auto i = 0u; i < m_population.size(); i++)
        {
            // m_fitness[i] = CalculateChromosomeFitness(i, fitnessCases);
            m_raffle.BuyTickets(CalculateChromosomeFitness(i, fitnessCases), i);
        }
    }

    double Population::CalculateChromosomeFitness(unsigned int index, const std::vector<std::vector<double>>& fitnessCases)
    {
        double sumOfSquares = 0.0;
        double caseFitness = 0.0;
        // auto& chromosome = m_population[index]; // the program of interest 
        for (auto& fCase : fitnessCases) // FitnessCases are the training set
        {
            m_allowedTerminals.clear();
            // for (auto i = 0u; i < std::tuple_size(fCase); i++)
            // {
                // m_allowedTerminals.push_back(&std::get<0>(fCase));
            // }
            // TODO:
            // calculate the fitness based on the variable values
            // and expected result for each case
            // i.e. caseFitness = (chromosome_answer - expected)^2
            caseFitness = 0.0;

            // add to the tally
            sumOfSquares += caseFitness;
        }
        // TODO: We want to minimize fitness, so should calculate the
        // inverse, 1/sumOfSquares... however we need to be careful to
        // avoid zero division errors, and overflow when adding doubles
        // that are close to the max double value.
        // return sumOfSquares;
        return 1.0;
    }

    std::tuple<INode*, INode*> Population::SelectParents()
    {
        int mum = m_raffle.Draw();
        int dad = m_raffle.Draw();
        return std::make_tuple( m_population[mum].get(), m_population[dad].get() );
    }
}
