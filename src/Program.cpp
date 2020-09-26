#include "Program.h"

#include <iostream>
#include <iomanip>
#include "ProgramSettings.h"
#include "model/FunctionFactory.h"
#include "model/Terminal.h"
#include "model/Operators.h"

namespace Model
{
    Program::Program()
        : m_logger("output.csv")
    {
        // parse command line input

        // TODO: things that need to be user defined are:
            // Allowed functions
            // Terminals
            // Fitness Cases
            // Crossover/Mutation probabilites
        // Create random/initial population from params
        // m_params.Seed = 0;

        m_params.AllowedFunctions = AllowedFunctions;
        m_params.NumberOfTerminals = static_cast<int>(FitnessCases[0].size()-1);
        m_population = std::make_unique<Population>(m_params);
    }

    void Program::Start()
    {
        // evolve over m_numGenerations
        for (auto i = 0; i < m_numGenerations; ++i)
        {
            m_population->CalculateFitness(FitnessCases);
            auto avg = m_population->GetAverageFitness();
            auto best = m_population->GetBestFitness();

            m_logger.AddLine(avg, best, "\"" + m_population->BestAsString() + "\"");

            m_population->Evolve();
        }

        m_population->CalculateFitness(FitnessCases);
        auto best = m_population->GetBestFitness();
        auto bestStr = m_population->BestAsString();
        m_logger.AddLine(m_population->GetAverageFitness(), best, "\"" + bestStr + "\"");

        // print best result
        std::cout << "Best S-expression has fitness: " << best << std::endl;
        std::cout << "\t" << bestStr << std::endl;
    }
}
