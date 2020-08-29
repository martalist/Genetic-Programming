#include "Program.h"

#include <iostream>
#include "ProgramSettings.h"
#include "model/FunctionFactory.h"
#include "model/Terminal.h"
#include "model/Operators.h"

namespace Model
{
    Program::Program()
    {
        // parse command line input

        // TODO: things that need to be user defined are:
            // Allowed functions
            // Terminals
            // Fitness Cases
            // Crossover/Mutation probabilites
        // Create random/initial population from params

        // temporary variable initialization
        double a = 1.0, b = 2.0, c = 3.0;
        m_params.AllowedTerminals.push_back(&a);
        m_params.AllowedTerminals.push_back(&b);
        m_params.AllowedTerminals.push_back(&c);

        m_params.AllowedFunctions = AllowedSets;
        m_population = std::make_unique<Population>(m_params);
    }

    void Program::Start()
    {
        // evolve over m_numGenerations
        for (auto i = 0; i < m_numGenerations; ++i)
        {
            m_population->CalculateFitness(FitnessCases);
            double avg = m_population->GetAverageFitness();
            double best = m_population->GetBestFitness();
            // TODO: log these parameters, for later analysis

            m_population->Evolve();
        }
        // m_population->CalculateFitness(FitnessCases);

        // TODO: report results
    }
}
