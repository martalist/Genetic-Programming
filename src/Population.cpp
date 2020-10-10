#include "Population.h"

// #include <iostream>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include "model/FunctionFactory.h"
#include "model/Terminal.h"
#include "model/Operators.h"

namespace
{
    using Chromosome = Model::Population::Chromosome;
    std::vector<Chromosome>::const_iterator GetBestIterator(const std::vector<Chromosome>& population)
    {
        auto best = std::min_element(population.begin(), population.end(),
                [](const Chromosome& a, const Chromosome& b) { return a.Fitness < b.Fitness; });

        if (best == population.end())
        {
            throw std::runtime_error("Can't get best fitness from empty population.");
        }
        return best;
    }
}

namespace Model
{
    using Chromosome = Population::Chromosome;

    Population::Population(const PopulationParams& params, const std::vector<std::vector<double>>& fitnessCases)
        : m_params(params)
        , m_randomProbability(.0, 1.)
        , m_terminals(params.NumberOfTerminals)
        , m_fitnessCases(fitnessCases)
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
        
        Reset();
    }

    void Population::Reset()
    {
        m_population.clear();
        m_raffle.Reset();

        // generate an appropriately sized population
        for (auto i = 0; i < m_params.PopulationSize; ++i)
        {
            auto itsABoy = Operators::CreateRandomChromosome(
                    m_params.MinInitialTreeSize, 
                    m_params.AllowedFunctions, 
                    m_allowedTerminals);
            m_population.emplace_back(std::move(itsABoy), 0.0);
        }
        CalculateFitness(m_population);
    }

    void Population::Evolve()
    {
        // Create a new population
        std::vector<Chromosome> newPopulation;
        while (newPopulation.size() < m_population.size())
        {
            // select a breeding pair
            auto [mum, dad] = SelectParents(); // raw pointers to Chromosome
            // std::cout << "parents:" << std::endl;
            // std::cout << "\tfitness: " << mum->Fitness << "\t" << mum->Tree->ToString() << std::endl;
            // std::cout << "\tfitness: " << dad->Fitness << "\t" << dad->Tree->ToString() << std::endl;

            // perform a deep copy, crossover & mutation
            Reproduce(*mum, *dad, newPopulation); // unique pointers
            // auto lastItr = newPopulation.end()-1;
            // std::cout << "children:" << std::endl;
            // std::cout << "\tfitness: " << CalculateChromosomeFitness(*lastItr->Tree) << "\t" << lastItr->Tree->ToString() << std::endl;
            // std::cout << "\tfitness: " << CalculateChromosomeFitness(*(lastItr-1)->Tree) << "\t" << (lastItr-1)->Tree->ToString() << std::endl << std::endl;
        }
        m_population.swap(newPopulation);

        // calculate the fitness of the new population
        CalculateFitness(m_population); 
    }

    void Population::Reproduce(const Chromosome& mum, const Chromosome& dad, std::vector<Chromosome>& nextGeneration)
    {
        // Deep copy mum & dad
        auto son = dad.Tree->Clone();
        auto daughter = mum.Tree->Clone();
        
        // should we crossover? 
        if (m_randomProbability.Get() <= m_params.CrossoverProb)
        {
            Operators::Crossover(son, daughter);
        }

        // should we mutate?
        if (m_randomProbability.Get() <= m_params.MutationProb)
        {
            Operators::Mutate(son, m_params.AllowedFunctions, m_allowedTerminals);
        }
        if (m_randomProbability.Get() <= m_params.MutationProb)
        {
            Operators::Mutate(daughter, m_params.AllowedFunctions, m_allowedTerminals);
        }

        if (m_params.AlwaysReplaceParents)
        {
            nextGeneration.emplace_back(std::move(son), 0.0);
            nextGeneration.emplace_back(std::move(daughter), 0.0);
        }
        else
        {
            if (CalculateChromosomeFitness(*son) < dad.Fitness)
            {
                nextGeneration.emplace_back(std::move(son), 0.0);
            }
            else
            {
                nextGeneration.emplace_back(dad.Tree->Clone(), 0.0);
            }

            if (CalculateChromosomeFitness(*daughter) < mum.Fitness)
            {
                nextGeneration.emplace_back(std::move(daughter), 0.0);
            }
            else
            {
                nextGeneration.emplace_back(mum.Tree->Clone(), 0.0);
            }
        }
    }
    
    void Population::CalculateFitness(std::vector<Chromosome>& population)
    {
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

        int i = 0;
        for (auto& chromosome : population)
        {
            chromosome.Fitness = CalculateChromosomeFitness(*chromosome.Tree);
            auto numberOfTickets = ticketAllocation(chromosome.Fitness);
            m_raffle.BuyTickets(numberOfTickets, i++);
        }
    }

    double Population::CalculateChromosomeFitness(const INode& chromosome)
    {
        double sumOfErrors = 0.0;
        for (auto& fCase : m_fitnessCases) // FitnessCases are the training set
        {
            for (auto i = 0u; i < fCase.size()-1; ++i)
            {
                m_terminals[i] = fCase[i];
            }

            // calculate the fitness (absolute error) for this fitness case
            auto returnVal = chromosome.Evaluate();
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

    std::tuple<Chromosome*, Chromosome*> Population::SelectParents()
    {
        int mum = m_raffle.Draw();
        int dad = m_raffle.Draw();
        return std::make_tuple( &m_population[mum], &m_population[dad] );
    }

    double Population::GetAverageFitness() const
    {
        int count = 0;
        double total = 0.0;
        for (auto i = 0u; i < m_population.size(); i++)
        {
            // Some functions have conditions that if not met result in NaN
            // For example, sqrt(a) requires that a >= 0. 
            // So only calculate the average of S-expressions that are defined across the entire terminal domain
            if (!std::isnan(m_population[i].Fitness) && m_population[i].Fitness != std::numeric_limits<double>::infinity())
            {
                total += m_population[i].Fitness;
                ++count;
            }
        }
        return total / count;
    }

    double Population::GetBestFitness() const
    {
        return GetBestIterator(m_population)->Fitness;
    }

    std::string Population::BestAsString() const
    {
        return GetBestIterator(m_population)->Tree->ToString();
    }
}
