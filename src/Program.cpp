#include "Program.h"

#include <iostream>
#include <iomanip>
#include "ConfigParser.h"
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

        // Get user configuration
        auto config = ConfigParser::Load("config.xml");
        m_params = config.Params;
        m_iterations = config.Iterations;
        m_numGenerations = config.NumGenerations;
        m_fitnessCases = config.FitnessCases;

        // Create random/initial population from params
        m_population = std::make_unique<Population>(m_params);
    }

    void Program::Start()
    {
        // Run the experiment m_iterations times
        for (int iteration = 0; iteration < m_iterations; ++iteration)
        {
            m_logger.SetFilename("output" + std::to_string(iteration));
            // m_logger.WriteHeader("Average Fitness", "Best Fitness", "Best S-Expression"); // TODO

            m_population->Reset(); // start with a fresh population

            // evolve over m_numGenerations
            for (auto i = 0; i < m_numGenerations; ++i)
            {
                m_population->CalculateFitness(m_fitnessCases);
                auto avg = m_population->GetAverageFitness();
                auto best = m_population->GetBestFitness();

                m_logger.AddLine(avg, best, "\"" + m_population->BestAsString() + "\"");

                m_population->Evolve();
            }

            m_population->CalculateFitness(m_fitnessCases);
            auto best = m_population->GetBestFitness();
            auto bestStr = m_population->BestAsString();
            m_logger.AddLine(m_population->GetAverageFitness(), best, "\"" + bestStr + "\"");
            m_logger.Write();

            // print best result
            std::cout << std::fixed << "Best S-expression in iteration " << iteration+1
                << " has fitness: " << best << std::endl
                << "\t" << bestStr << std::endl << std::endl;
        }
        std::cout << "Results written to " << m_logger.GetOutputDir() << std::endl << std::endl;
    }
}
