#include "Population.h"

#include <algorithm>
#include <cmath>
// #include <iostream>
#include <stdexcept>
#include "model/FunctionFactory.h"
#include "model/ChromosomeFactory.h"
#include "model/ChromosomeUtil.h"
#include "utils/Math.h"
#include "utils/Raffle.h"
#include "utils/Tournament.h"

namespace
{
    // The size of the tournament for individual parent selection
    const int TournamentSize = 20; 

    // The default parsimony coefficient
    const double DefaultParsimonyCoefficient = 0.025;

    // The number of twins to produce from each mating pair. Only 2 children will survive
    // so the mortality rate is impied.
    const int TwinsPerMatingPair = 3;

    // Utility function for ordering the population
    const auto ChromoPtrOrder = [] (const Model::Population::ChromoPtr& a, const Model::Population::ChromoPtr& b) 
    { 
        return *a < *b; 
    };

    /**
     * Returns the number of raffle tickets should be allocated for a given fitness value.
     * The smaller (better) the fitness, the higher the ticket number. 
     * This is only used in conjunction with Raffle ticketing (i.e. not Tournament).
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
}

namespace Model
{
    Population::Population(const PopulationParams& params, const std::vector<double>& fitnessCases)
        : m_params(params)
        , m_randomProbability(.0, 1.)
        // TODO: allow config to select between raffle and tournament style selection
        // , m_selector(std::make_unique<Util::Raffle<double>>())
        // TODO: make the tournament size configurable by XML
        , m_selector(std::make_unique<Util::Tournament<double>>(m_params.PopulationSize, TournamentSize))
        , m_terminals(params.NumberOfTerminals)
        , m_fitnessCases(fitnessCases)
    {
        if (params.Seed.has_value())
        {
            ChromosomeUtil::SetSeed(params.Seed.value());
            m_randomProbability.SetSeed(params.Seed.value());
            m_selector->SetSeed(params.Seed.value());
        }

        for (auto& terminal : m_terminals)
        {
            m_allowedTerminals.push_back(&terminal);
        }

        ChromosomeFactory::Initialise(m_params.Type, m_params.MinInitialTreeSize, 
                m_params.AllowedFunctions, m_allowedTerminals, m_fitnessCases, m_terminals);
    }

    void Population::Reset()
    {
        m_population.clear();
        m_selector->Reset();

        // generate an appropriately sized population
        for (auto i = 0; i < m_params.PopulationSize; ++i)
        {
            m_population.push_back(ChromosomeFactory::Inst().CreateRandom(m_parsimonyCoefficient));
        }
        RecalibrateParentSelector(); // TODO
    }

    void Population::Evolve()
    {
        // Create a new population
        std::vector<Population::ChromoPtr> newPopulation;
        while (newPopulation.size() < m_population.size())
        {
            // select a breeding pair
            auto [mum, dad] = SelectParents(); // raw pointers to Chromosome
            // std::cout << "parents:" << std::endl;
            // std::cout << "\tfitness: " << mum->Fitness() << "\t" << mum->GetTree()->ToString() << std::endl;
            // std::cout << "\tfitness: " << dad->Fitness() << "\t" << dad->GetTree()->ToString() << std::endl;

            // perform a deep copy, crossover & mutation
            Reproduce(*mum, *dad, newPopulation); // unique pointers
            // auto lastItr = newPopulation.end()-1;
            // std::cout << "children:" << std::endl;
            // std::cout << "\tfitness: " << CalculateChromosomeFitness(*lastItr->GetTree()) << "\t" << lastItr->GetTree()->ToString() << std::endl;
            // std::cout << "\tfitness: " << CalculateChromosomeFitness(*(lastItr-1)->GetTree()) << "\t" << (lastItr-1)->GetTree()->ToString() << std::endl << std::endl;
        }
        m_population.swap(newPopulation);

        // calculate the fitness of the new population
        RecalibrateParentSelector(); 
    }

    void Population::Reproduce(const IChromosome& mum, const IChromosome& dad, std::vector<Population::ChromoPtr>& nextGeneration)
    {
        // TODO: this is no longer useful. Would be better to specify the number twins/pairs per set of parents.
        if (m_params.AlwaysReplaceParents)
        {
            auto [son, daughter] = GetNewOffspring(mum, dad, m_fitnessCases, m_terminals, m_parsimonyCoefficient);
            nextGeneration.push_back(std::move(son));
            nextGeneration.push_back(std::move(daughter));
        }
        else
        {
            std::vector<Population::ChromoPtr> family;
            for (int i = 0; i < TwinsPerMatingPair; ++i)
            {
                auto [son, daughter] = GetNewOffspring(mum, dad, m_fitnessCases, m_terminals, m_parsimonyCoefficient);
                family.push_back(std::move(son));
                family.push_back(std::move(daughter));
            }
            std::sort(family.begin(), family.end(), ChromoPtrOrder);

            auto inOrder = family.begin();
            nextGeneration.push_back(std::move(*inOrder));
            ++inOrder;
            nextGeneration.push_back(std::move(*inOrder));
        }
    }

    std::tuple<Population::ChromoPtr, Population::ChromoPtr> Population::GetNewOffspring(const IChromosome& mum, const IChromosome& dad, const std::vector<double>& fitnessCases, std::vector<double>& terminals, double parsimonyCoefficient) const
    {
        // Deep copy mum & dad
        auto son = dad.Clone();
        auto daughter = mum.Clone();

        // should we crossover? 
        if (m_randomProbability.Get() <= m_params.CrossoverProb)
        {
            son->Crossover(*daughter);
        }

        // should we mutate son?
        auto mutationLikelihood = m_randomProbability.Get();
        if (mutationLikelihood <= m_params.MutationProb)
        {
            son->Mutate(m_params.AllowedFunctions, m_allowedTerminals);
        }
        else if (mutationLikelihood <= m_params.MutationProb + m_params.HoistMutationProb)
        {
            son->HoistMutate();
        }

        // should we mutate daughter?
        mutationLikelihood = m_randomProbability.Get();
        if (mutationLikelihood <= m_params.MutationProb)
        {
            daughter->Mutate(m_params.AllowedFunctions, m_allowedTerminals);
        }
        else if (mutationLikelihood <= m_params.MutationProb + m_params.HoistMutationProb)
        {
            daughter->HoistMutate();
        }

        return 
        {
            ChromosomeFactory::Inst().CopyAndEvaluate(std::move(son->GetTree()), parsimonyCoefficient),
            ChromosomeFactory::Inst().CopyAndEvaluate(std::move(daughter->GetTree()), parsimonyCoefficient),
        };
    }

    void Population::RecalibrateParentSelector()
    {
        SortPopulation();

        m_selector->Reset(); // get rid of the previous generation's tickets
        
        int i = 0;
        for (auto& chromosome : m_population)
        {
            // TODO: only calculate ticketAllocation for raffle style
            // auto numberOfTickets = ticketAllocation(chromosome.Fitness());
            auto numberOfTickets = chromosome->Fitness();
            m_selector->RegisterElement(numberOfTickets, i);
            ++i;
        }
        m_parsimonyCoefficient = UpdateParsimonyCoefficient();
        // std::cout << "Parsimony Coefficient = " << m_parsimonyCoefficient << std::endl;
    }
    
    void Population::SortPopulation()
    {
        // sort primary population by WeightedFitness
        std::sort(m_population.begin(), m_population.end(), ChromoPtrOrder);

        // sort pointers by fitness
        m_sortedByFitness.clear();
        for (auto& p : m_population)
        {
            m_sortedByFitness.push_back(p.get());
        }
        std::sort(m_sortedByFitness.begin(), m_sortedByFitness.end(), [](const IChromosome* a, const IChromosome* b) { return a->Fitness() < b->Fitness(); });
    }

    std::tuple<IChromosome*, IChromosome*> Population::SelectParents()
    {
        int mum = m_selector->Draw();
        int dad = m_selector->Draw();
        return std::make_tuple( m_population[mum].get(), m_population[dad].get() );
    }

    std::tuple<double, double, double, double, double> Population::GetRangeStatistics() const
    {
        auto size = m_population.size();
        auto min = m_sortedByFitness[0]->Fitness();
        auto max = m_sortedByFitness[size-1]->Fitness();

        auto getMedian = [&](int begin, int end)
        {
            auto size = end - begin;
            auto middle = begin + size/2;
            double median = m_sortedByFitness[middle]->Fitness();
            if (size % 2 == 0) // even
            {
                auto middleLeft = m_sortedByFitness[middle-1]->Fitness();
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
        using ConstItr = std::vector<Population::ChromoPtr>::const_iterator;
        auto getFitness = [](const ConstItr& itr) { return (*itr)->Fitness(); };
        return Util::Average<ConstItr>(m_population.begin(), m_population.end(), getFitness);
    }

    std::string Population::BestAsString() const
    {
        return m_sortedByFitness[0]->ToString();
    }

    double Population::UpdateParsimonyCoefficient()
    {
        return DefaultParsimonyCoefficient;  // TODO: disabling this is a good idea for TS

        // TODO: This implementation of dynamic parsimony coefficient calculation does not yield
        // the desired result. So for now we're returning the default value above.
        using Itr = std::vector<Population::ChromoPtr>::iterator;
        const double DenominatorThreshold = 1e-06;

        auto getSize = [](const Itr& itr) -> double { return static_cast<double>((*itr)->Size()); };
        auto getFitness = [](const Itr& itr) -> double { return (*itr)->Fitness(); };
        
        auto varSize = Util::Variance<Itr>(m_population.begin(), m_population.end(), getSize);
        auto covar = Util::Covariance<Itr, Itr>( m_population.begin(), m_population.end(), getSize,
                                            m_population.begin(), m_population.end(), getFitness);

        // TODO: Revisit the paper documenting this method. It may be affected by allowing > 2 children
        // per node.
        if (covar < DenominatorThreshold)
        {
            if (varSize < DenominatorThreshold)
            {
                return 0.0; // approaching 0/0
            }
            return 1.0; // approaching infty
        }
        return covar / varSize;
    }
}
