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

    void Program::TemporaryTesting()
    {
        double a = 1.0;
        double b = 2.2;

        auto aVar = std::make_unique<Variable>(a);
        auto bVar = std::make_unique<Variable>(b);
        std::cout << "a = " << a << " = " << aVar->Evaluate() << std::endl;
        std::cout << "b = " << b << " = " << bVar->Evaluate() << std::endl;
        
        // test that the reference works
        b = 2.5;
        std::cout << "b = " << b << " = " << bVar->Evaluate() << std::endl;

        auto add = FunctionFactory::Create(FunctionType::Addition);
        add->AddChild(std::make_unique<Variable>(a));
        add->AddChild(std::make_unique<Variable>(b));
        std::cout << "a + b = " << a << " + " << b << " = " 
            << add->Evaluate() << std::endl;

        auto sub = FunctionFactory::Create(FunctionType::Subtraction);
        sub->AddChild(std::make_unique<Variable>(a));
        sub->AddChild(std::make_unique<Variable>(b));
        std::cout << "a - b = " << sub->Evaluate() << std::endl;

        auto mult = FunctionFactory::Create(FunctionType::Multiplication);
        mult->AddChild(std::make_unique<Variable>(a));
        mult->AddChild(std::make_unique<Variable>(b));
        std::cout << "a * b = " << a << " * " << b << " = " 
            << mult->Evaluate() << std::endl;

        auto div = FunctionFactory::Create(FunctionType::Division);
        div->AddChild(std::make_unique<Variable>(a));
        div->AddChild(std::make_unique<Variable>(b));
        std::cout << "a / b = " << a << " / " << b << " = " 
            << div->Evaluate() << std::endl;

        // auto sq = FunctionFactory::Create(FunctionType::SquareRoot);
        // sq->AddChild(std::move(bVar));
        // std::cout << "sqrt(b) = " << sq->Evaluate() << std::endl;

        auto nested = FunctionFactory::Create(FunctionType::SquareRoot);
        auto squared = FunctionFactory::Create(FunctionType::Multiplication);
        squared->AddChild(std::make_unique<Variable>(b));
        squared->AddChild(std::make_unique<Variable>(b));
        squared->AddChild(std::make_unique<Variable>(b));
        squared->AddChild(std::make_unique<Variable>(b));
        nested->AddChild(std::move(squared));
        std::cout << "sqrt(b*b) = " << nested->Evaluate() << std::endl;
    }
    
    void Program::Start()
    {
        TemporaryTesting();
        
        // generate m_populationSize chromosomes
        for (auto i = 0u; i < m_populationSize; ++i)
        {
            auto itsABoy = Operators::CreateRandomChromosome(m_minInitialTreeSize, m_allowedFunctions);
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
        auto son      = FunctionFactory::Copy(dad);
        auto daughter = FunctionFactory::Copy(mum);
        
        // should we crossover? 
        if (m_randomProbability.Get() <= m_crossoverProb)
        {
            Operators::Crossover(*son, *daughter);
        }

        // should we mutate?
        if (m_randomProbability.Get() <= m_mutationProb)
        {
            Operators::Mutate(*son);
        }
        if (m_randomProbability.Get() <= m_mutationProb)
        {
            Operators::Mutate(*daughter);
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
            // TODO:
            // calculate the fitness based on the variable values
            // and expected result for each case
            // i.e. caseFitness = (chromosome_answer - expected)^2
            caseFitness = 0.0;

            // add to the tally
            sumOfSquares += caseFitness;
        }
        return sumOfSquares; // this is what we want to minimize
    }

    std::tuple<INode*, INode*> Program::SelectParents()
    {
        int mum = m_raffle.Draw();
        int dad = m_raffle.Draw();
        return std::make_tuple( m_population[mum].get(), m_population[dad].get() );
    }
}
