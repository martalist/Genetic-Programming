#include "Population.h"

#include <algorithm>
#include <cmath>
// #include <iostream>
#include <stdexcept>
#include "model/FunctionFactory.h"
#include "model/Terminal.h"
#include "model/Operators.h"
#include "utils/Math.h"
#include "utils/Raffle.h"
#include "utils/Tournament.h"

namespace Model
{
    using INodePtr = Population::INodePtr;

    Population::Population(const PopulationParams& params, const std::vector<std::vector<double>>& fitnessCases)
        : m_params(params)
        , m_randomProbability(.0, 1.)
        // TODO: allow config to select between raffle and tournament style selection
        // , m_selector(std::make_unique<Util::Raffle<double>>())
        // TODO: make the tournament size configurable by XML
        , m_selector(std::make_unique<Util::Tournament<double>>(m_params.PopulationSize, 20))
        , m_terminals(params.NumberOfTerminals)
        , m_fitnessCases(fitnessCases)
    {
        if (params.Seed.has_value())
        {
            Operators::SetSeed(params.Seed.value());
            m_randomProbability.SetSeed(params.Seed.value());
            m_selector->SetSeed(params.Seed.value());
        }

        for (auto& terminal : m_terminals)
        {
            m_allowedTerminals.push_back(&terminal);
        }
    }

    void Population::Reset()
    {
        m_population.clear();
        m_selector->Reset();

        // generate an appropriately sized population
        for (auto i = 0; i < m_params.PopulationSize; ++i)
        {
            auto itsABoy = Operators::CreateRandomChromosome(
                    m_params.MinInitialTreeSize, 
                    m_params.AllowedFunctions, 
                    m_allowedTerminals);
            m_population.emplace_back(std::move(itsABoy), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
        }
        RecalibrateParentSelector(); // TODO
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
        RecalibrateParentSelector(); 
    }

    void Population::Reproduce(const Chromosome& mum, const Chromosome& dad, std::vector<Chromosome>& nextGeneration)
    {
        auto [son, daughter] = GetNewOffspring(mum, dad);

        if (m_params.AlwaysReplaceParents)
        {
            nextGeneration.emplace_back(std::move(son), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            nextGeneration.emplace_back(std::move(daughter), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
        }
        else
        {
            // add a couple more kiddos to the mix
            auto [s2, d2] = GetNewOffspring(mum, dad);
            auto [s3, d3] = GetNewOffspring(mum, dad);
            std::vector<Chromosome> family;
            // family.emplace_back(dad.Tree->Clone(), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            // family.emplace_back(mum.Tree->Clone(), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            family.emplace_back(std::move(son), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            family.emplace_back(std::move(daughter), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            family.emplace_back(std::move(s2), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            family.emplace_back(std::move(d2), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            family.emplace_back(std::move(s3), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            family.emplace_back(std::move(d3), m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            std::sort(family.begin(), family.end());

            auto inOrder = family.begin();
            auto fitness = inOrder->Fitness;
            nextGeneration.emplace_back(std::move(inOrder->Tree), fitness, m_parsimonyCoefficient);
            ++inOrder;
            fitness = inOrder->Fitness;
            nextGeneration.emplace_back(std::move(inOrder->Tree), fitness, m_parsimonyCoefficient);
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

        // should we mutate son?
        auto mutationLikelihood = m_randomProbability.Get();
        if (mutationLikelihood <= m_params.MutationProb)
        {
            Operators::Mutate(son, m_params.AllowedFunctions, m_allowedTerminals);
        }
        else if (mutationLikelihood <= m_params.MutationProb + m_params.HoistMutationProb)
        {
            Operators::HoistMutate(son);
        }

        // should we mutate daughter?
        mutationLikelihood = m_randomProbability.Get();
        if (mutationLikelihood <= m_params.MutationProb)
        {
            Operators::Mutate(daughter, m_params.AllowedFunctions, m_allowedTerminals);
        }
        else if (mutationLikelihood <= m_params.MutationProb + m_params.HoistMutationProb)
        {
            Operators::HoistMutate(daughter);
        }

        return std::make_tuple(std::move(son), std::move(daughter));
    }

    void Population::RecalibrateParentSelector()
    {
        SortPopulation();

        m_selector->Reset(); // get rid of the previous generation's tickets
        
        /**
         * Returns the number of raffle tickets should be allocated for a given fitness value.
         * The smaller (better) the fitness, the higher the ticket number. 
         */
        // auto ticketAllocation = [](double fitness) -> double 
        // {
            // if (fitness == std::numeric_limits<double>::infinity())
            // {
                // return 0.0;
            // }
            // // return std::min(1.0/fitness, std::numeric_limits<double>::max()/10'000);
            // return std::exp(4.0 - std::pow(fitness, 2.0/5.0));
            // // return std::exp(7.0 - std::pow(fitness, 0.32));
        // };

        int i = 0;
        for (auto& chromosome : m_population)
        {
            // TODO: only calculate ticketAllocation for raffle style
            // auto numberOfTickets = ticketAllocation(chromosome.Fitness);
            auto numberOfTickets = chromosome.Fitness;
            m_selector->RegisterElement(numberOfTickets, i++);
        }
        m_parsimonyCoefficient = UpdateParsimonyCoefficient();
        // std::cout << "Parsimony Coefficient = " << m_parsimonyCoefficient << std::endl;
    }
    
    void Population::SortPopulation()
    {
        // sort primary population by WeightedFitness
        std::sort(m_population.begin(), m_population.end());

        // sort pointers by fitness
        m_sortedByFitness.clear();
        for (auto& p : m_population)
        {
            m_sortedByFitness.push_back(&p);
        }
        std::sort(m_sortedByFitness.begin(), m_sortedByFitness.end(), [](const Chromosome* a, const Chromosome* b) { return a->Fitness < b->Fitness; });
    }

    std::tuple<Chromosome*, Chromosome*> Population::SelectParents()
    {
        int mum = m_selector->Draw();
        int dad = m_selector->Draw();
        return std::make_tuple( &m_population[mum], &m_population[dad] );
    }

    std::tuple<double, double, double, double, double> Population::GetRangeStatistics() const
    {
        auto size = m_population.size();
        auto min = m_sortedByFitness[0]->Fitness;
        auto max = m_sortedByFitness[size-1]->Fitness;

        auto getMedian = [&](int begin, int end)
        {
            auto size = end - begin;
            auto middle = begin + size/2;
            double median = m_sortedByFitness[middle]->Fitness;
            if (size % 2 == 0) // even
            {
                auto middleLeft = m_sortedByFitness[middle-1]->Fitness;
                // if even, need to return the average of the middle two elems
                median = (middleLeft + median) * 0.5;
            }
            return median;
        };

        auto median = getMedian(0, size);
        auto firstQuartile = getMedian(0, size/2);
        auto thirdQuartile = getMedian(size/2, size);

        return std::make_tuple(min, firstQuartile, median, thirdQuartile, max);
    }

    double Population::GetAverageFitness() const
    {
        using ConstItr = std::vector<Chromosome>::const_iterator;
        auto getFitness = [](const ConstItr& itr) { return itr->Fitness; };
        return Util::Average<ConstItr>(m_population.begin(), m_population.end(), getFitness);
    }

    std::string Population::BestAsString() const
    {
        return m_sortedByFitness[0]->Tree->ToString();
    }

    double Population::UpdateParsimonyCoefficient()
    {
        using Itr = std::vector<Chromosome>::iterator;
        const double DenominatorThreshold = 1e-06;

        auto getSize = [](const Itr& itr) -> double { return static_cast<double>(itr->Size); };
        auto getFitness = [](const Itr& itr) -> double { return itr->Fitness; };
        
        auto varSize = Util::Variance<Itr>(m_population.begin(), m_population.end(), getSize);
        auto covar = Util::Covariance<Itr, Itr>( m_population.begin(), m_population.end(), getSize,
                                            m_population.begin(), m_population.end(), getFitness);

        return 0.025; 
        // TODO: this may be affected by allowing > 2 children per function
        if (covar < DenominatorThreshold)
        {
            if (varSize < DenominatorThreshold)
            {
                return 0.0; // approaching 0/0
            }
            return 1.0; // approaching infty
        }
        return covar / varSize;
        return std::clamp(covar / varSize, 0.0, 1.0);
    }
}
