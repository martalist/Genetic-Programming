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
    {
        // parse command line input

        // TODO: things that need to be user defined are:
            // Allowed functions
            // Terminals
            // Fitness Cases
            // Crossover/Mutation probabilites
        // Create random/initial population from params
        // m_params.Seed = 0;

        m_params.AllowedFunctions = AllowedSets;
        m_params.NumberOfTerminals = static_cast<int>(FitnessCases[0].size()-1);
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

            std::cout << std::fixed << std::setprecision(2) << "Generation: " << i
                << ", avg = " << avg << ", best = " << best << " [[ " << m_population->BestAsString() << " ]]"<< std::endl;

            m_population->Evolve();
        }
        // m_population->CalculateFitness(FitnessCases);

        // print best result
        std::cout << m_population->BestAsString() << std::endl;
        
        // TODO: report results
        // Should be able to get all or top-N S-expressions.
    }
}
