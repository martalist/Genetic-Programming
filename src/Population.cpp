#include "Population.h"

#include <algorithm>
#include <cmath>
// #include <iostream>
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
    using INodePtr = Population::INodePtr;

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
        // CalculateFitness(m_population); 
    }

    void Population::Reproduce(const Chromosome& mum, const Chromosome& dad, std::vector<Chromosome>& nextGeneration)
    {
        auto [son, daughter] = GetNewOffspring(mum, dad);

        if (m_params.AlwaysReplaceParents)
        {
            nextGeneration.emplace_back(std::move(son), 0.0);
            nextGeneration.emplace_back(std::move(daughter), 0.0);
        }
        else
        {
            // Use a set to order the family, and pick the best to survive
            auto CompareINodePtr = [&](const Chromosome& a, const Chromosome& b) -> bool
            { 
                return a.Fitness < b.Fitness;
            };

            // add a couple more kiddos to the mix
            // auto [s2, d2] = GetNewOffspring(mum, dad);
            // auto [s3, d3] = GetNewOffspring(mum, dad);
            std::vector<Chromosome> family;
            family.emplace_back(dad.Tree->Clone(), CalculateChromosomeFitness(*dad.Tree));
            family.emplace_back(mum.Tree->Clone(), CalculateChromosomeFitness(*mum.Tree));
            family.emplace_back(std::move(son), CalculateChromosomeFitness(*son));
            family.emplace_back(std::move(daughter), CalculateChromosomeFitness(*daughter));
            // family.emplace_back(std::move(s2), CalculateChromosomeFitness(*s2));
            // family.emplace_back(std::move(s3), CalculateChromosomeFitness(*s3));
            // family.emplace_back(std::move(d2), CalculateChromosomeFitness(*d2));
            // family.emplace_back(std::move(d3), CalculateChromosomeFitness(*d3));
            std::sort(family.begin(), family.end(), CompareINodePtr);

            auto inOrder = family.begin();
            nextGeneration.emplace_back(std::move(inOrder->Tree), inOrder->Fitness);
            ++inOrder;
            nextGeneration.emplace_back(std::move(inOrder->Tree), inOrder->Fitness);
        }
    }

    std::tuple<INodePtr, INodePtr> Population::GetNewOffspring(const Chromosome& mum, const Chromosome& dad) const
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
        return std::make_tuple(std::move(son), std::move(daughter));
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
            // return std::min(1.0/fitness, std::numeric_limits<double>::max()/10'000);
            return std::exp(4.0 - std::pow(fitness, 2.0/5.0));
            // return std::exp(7.0 - std::pow(fitness, 0.32));
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
        for (const auto& fCase : m_fitnessCases) // FitnessCases are the training set
        {
            for (auto i = 0u; i < fCase.size()-1; ++i)
            {
                m_terminals[i] = fCase[i];
            }

            // calculate the fitness (absolute error) for this fitness case
            auto returnVal = chromosome.Evaluate();
            if (std::isnan(returnVal)) // TODO: with closure changes (237c984) this shouldn't be necessary.
            {
                // eliminate it's chances of reproduction
                return std::numeric_limits<double>::infinity();
            }

            // add to the tally
            sumOfErrors += std::abs(returnVal - fCase.back());
        }
        return sumOfErrors / m_fitnessCases.size(); // mean absolute error
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
