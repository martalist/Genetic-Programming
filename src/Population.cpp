#include "Population.h"

#include <algorithm>
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
        // TODO: min or max element ?? 
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
        if (params.Seed)
        {
            Operators::SetSeed(params.Seed.value());
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
        m_fitness.clear();
        m_raffle.Reset(); // get rid of the previous generation's tickets
        for (auto i = 0u; i < m_population.size(); i++)
        {
            m_fitness.push_back(CalculateChromosomeFitness(i, fitnessCases));

            // TODO: We want to minimize fitness, so should calculate the
            // inverse, 1/m_fitness... however we need to be careful to
            // avoid zero division errors, and overflow when adding doubles
            // that are close to the max double value.
            m_raffle.BuyTickets(1.0/m_fitness[i], i);
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

            // calculate the fitness based on the variable values
            // and expected result for each case
            auto caseFitness = m_population[index]->Evaluate() - fCase.back();

            // add to the tally
            sumOfErrors += caseFitness;
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
        auto total = std::accumulate(m_fitness.begin(), m_fitness.end(), 0.0,
                [](double result, double fitness) { return result + fitness; });
        return total / static_cast<double>(m_fitness.size());
    }

    double Population::GetBestFitness() const
    {
        auto best = GetBestIterator(m_fitness);
        return *best;
    }

    std::string Population::BestAsString() const
    {
        auto best = GetBestIterator(m_fitness);
        int index = best - m_fitness.begin();
        return m_population[index]->ToString();
    }
}
