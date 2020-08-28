#include "Program.h"

#include <iostream>
#include "ProgramSettings.h"
#include "model/FunctionFactory.h"
#include "model/Variable.h"
#include "model/Operators.h"

namespace Model
{
    Program::Program()
        : m_fitness(0.0, m_populationSize)
        , m_randomProbability(.0, 1.)
        , m_allowedFunctions(AllowedSets)
    {
        // parse command line input

        m_population.reserve(m_populationSize); // make room
    }

    void Program::Start()
    {
        // temporary variable initialization
        double a = 1.0, b = 2.0, c = 3.0;
        m_variables.push_back(&a);
        m_variables.push_back(&b);
        m_variables.push_back(&c);

        // generate m_populationSize chromosomes
        for (auto i = 0u; i < m_populationSize; ++i)
        {
            auto itsABoy = Operators::CreateRandomChromosome(m_minInitialTreeSize, m_allowedFunctions, m_variables);
            m_population.push_back(std::move(itsABoy));
        }

        // evolve over m_numGenerations
        for (auto i = 0u; i < m_numGenerations; ++i)
        {
            // evaluate the fitness
            CalculatePopulationFitness();
        
            // breed
            ProduceNextGeneration();
        }
        CalculatePopulationFitness();

        // TODO: report results
    }

    void Program::ProduceNextGeneration()
    {
        std::vector<std::unique_ptr<INode>> newPopulation;
        while (newPopulation.size() < m_populationSize)
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

    Program::NodePair Program::Reproduce(const INode& mum, const INode& dad)
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
            Operators::Mutate(son, m_allowedFunctions, m_variables);
        }
        if (m_randomProbability.Get() <= m_mutationProb)
        {
            Operators::Mutate(daughter, m_allowedFunctions, m_variables);
        }

        return std::make_tuple(std::move(son), std::move(daughter));
    }
    
    void Program::CalculatePopulationFitness()
    {
        m_raffle.Reset(); // get rid of the previous generation's tickets
        for (auto i = 0u; i < m_populationSize; i++)
        {
            m_fitness[i] = CalculateChromosomeFitness(i);
            m_raffle.BuyTickets(m_fitness[i], i);
        }
    }

    double Program::CalculateChromosomeFitness(unsigned int index)
    {
        double sumOfSquares = 0.0;
        double caseFitness = 0.0;
        auto& chromosome = m_population[index]; // the program of interest 
        for (auto& fCase : FitnessCases) // FitnessCases are the training set
        {
            std::get<0>(fCase); // just shupping up the compiler
            chromosome->Size(); // just shupping up the compiler
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
        return sumOfSquares;
    }

    std::tuple<INode*, INode*> Program::SelectParents()
    {
        int mum = m_raffle.Draw();
        int dad = m_raffle.Draw();
        return std::make_tuple( m_population[mum].get(), m_population[dad].get() );
    }
}
