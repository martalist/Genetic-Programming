#include "Population.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include "PopulationParams.h"
#include "model/FunctionFactory.h"
#include "model/Terminal.h"
#include "model/Operators.h"

namespace
{
    std::vector<double>::const_iterator GetBestIterator(const std::vector<double>& vec)
    {
        auto best = std::min_element(vec.begin(), vec.end());
        if (best == vec.end())
        {
            throw std::runtime_error("Can't get best fitness from empty fitness vector.");
        }
        return best;
    }
}

namespace Model
{
    Population::Population(const PopulationParams& params)
        : m_crossoverProb(params.CrossoverProb)
        , m_mutationProb(params.MutationProb)
        , m_randomProbability(.0, 1.)
        , m_allowedFunctions(params.AllowedFunctions)
        , m_terminals(params.NumberOfTerminals)
    {
        if (params.Seed.has_value())
        {
            Operators::SetSeed(params.Seed.value());
            m_randomProbability.SetSeed(params.Seed.value());
            m_raffle.SetSeed(params.Seed.value());
        }

        for (auto& terminal : m_terminals)
        {
            m_allowedTerminals.push_back(&terminal);
        }
        
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
        m_population.swap(newPopulation);
    }

    Population::NodePair Population::Reproduce(const INode& mum, const INode& dad)
    {
        // Deep copy mum & dad
        auto son = dad.Clone();
        auto daughter = mum.Clone();
        
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
        m_fitness.clear();
        m_raffle.Reset(); // get rid of the previous generation's tickets
        
        /**
         * Returns the number of raffle tickets should be allocated for a given fitness value.
         * The smaller (better) the fitness, the higher the ticket number. 
         */
        auto ticketAllocation = [](double fitness) -> double 
        {
            if (fitness == std::numeric_limits<double>::infinity())
            {
                return 0.0;
            }
            return std::exp(4.0 - std::pow(fitness, 2.0/5.0));
        };

        for (auto i = 0u; i < m_population.size(); i++)
        {
            m_fitness.push_back(CalculateChromosomeFitness(i, fitnessCases));

            auto numberOfTickets = ticketAllocation(m_fitness.back());
            m_raffle.BuyTickets(numberOfTickets, i);
        }
    }

    double Population::CalculateChromosomeFitness(unsigned int index, const std::vector<std::vector<double>>& fitnessCases)
    {
        double sumOfErrors = 0.0;
        for (auto& fCase : fitnessCases) // FitnessCases are the training set
        {
            for (auto i = 0u; i < fCase.size()-1; i++)
            {
                m_terminals[i] = fCase[i];
            }

            // calculate the fitness (absolute error) for this fitness case
            auto returnVal = m_population[index]->Evaluate();
            if (std::isnan(returnVal)) // could be sqrt(-1)
            {
                // eliminate it's chances of reproduction
                return std::numeric_limits<double>::infinity();
            }

            // add to the tally
            sumOfErrors += std::abs(returnVal - fCase.back());
        }
        return sumOfErrors;
    }

    std::tuple<INode*, INode*> Population::SelectParents()
    {
        int mum = m_raffle.Draw();
        int dad = m_raffle.Draw();
        return std::make_tuple( m_population[mum].get(), m_population[dad].get() );
    }

    double Population::GetAverageFitness() const
    {
        int count = 0;
        double total = 0.0;
        for (auto i = 0u; i < m_fitness.size(); i++)
        {
            // Some functions have conditions that if not met result in NaN
            // For example, sqrt(a) requires that a >= 0. 
            // So only calculate the average of S-expressions that are defined across the entire terminal domain
            if (!std::isnan(m_fitness[i]) && m_fitness[i] != std::numeric_limits<double>::infinity())
            {
                total += m_fitness[i];
                ++count;
            }
        }
        return total / count;
    }

    double Population::GetBestFitness() const
    {
        return *GetBestIterator(m_fitness);
    }

    std::string Population::BestAsString() const
    {
        auto best = GetBestIterator(m_fitness);
        auto index = std::distance(m_fitness.begin(), best);
        return m_population[index]->ToString();
    }
}
