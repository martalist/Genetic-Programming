#include "Program.h"

#include <iostream>
#include <iomanip>
#include "ConfigParser.h"
#include "model/FunctionFactory.h"

namespace Model
{
    Program::Program()
    {
        // Get user configuration
        auto config = ConfigParser::Load("config.xml");
        m_params = config.Params;
        m_iterations = config.Iterations;
        m_numGenerations = config.NumGenerations;
        if (config.StoppingCriteria.has_value())
        {
            m_stoppingCriteria = config.StoppingCriteria.value();
        }

        // Create random/initial population from params
        m_population = std::make_unique<Population>(config.Params, config.FitnessCases);
    }

    Program::Program(const Config& config)
        : m_params(config.Params)
        , m_population(std::make_unique<Population>(config.Params, config.FitnessCases))
        , m_numGenerations(config.NumGenerations)
        , m_iterations(config.Iterations)
        , m_stoppingCriteria(config.StoppingCriteria)
    {
    }

    void Program::Start(bool logResults)
    {
        // Run the experiment m_iterations times
        for (int iteration = 0; iteration < m_iterations; ++iteration)
        {
            m_logger.SetFilename("output" + std::to_string(iteration));
            // m_logger.WriteHeader("Average Fitness", "Best Fitness", "Best S-Expression"); // TODO

            m_population->Reset(); // start with a fresh population
            auto [ min, firstQtr, median, thirdQtr, max ] = m_population->GetRangeStatistics();
            m_logger.AddLine(min, firstQtr, median, thirdQtr, max, "\"" + m_population->GetBestFit()->ToString() + "\"");

            // evolve over m_numGenerations
            for (auto i = 0; i < m_numGenerations; ++i)
            {
                m_population->Evolve();
                auto [ minimum, firstQtr, median, thirdQtr, max ] = m_population->GetRangeStatistics();
                min = minimum;

                m_logger.AddLine(minimum, firstQtr, median, thirdQtr, max, "\"" + m_population->GetBestFit()->ToString() + "\"");

                if (m_stoppingCriteria.has_value() && minimum < m_stoppingCriteria.value())
                {
                    break;
                }
            }

            // print best result
            std::cout << std::fixed << "Best S-expression in iteration " << iteration+1
                << " has fitness: " << min << std::endl
                << "\t" << m_population->GetBestFit()->ToString() << std::endl << std::endl;
        }

        if (logResults)
        {
            std::cout << "Results written to " << m_logger.GetOutputDir() << std::endl << std::endl;
        }
    }

    double Program::Predict(double* predictions, int length)
    {
        if (m_params.Type != ChromosomeType::TimeSeries)
        {
            throw std::invalid_argument("Forecasting can only be performed on time series data.");
        }

        m_population->Predict(predictions, length);
        return m_population->GetBestFit()->Fitness();
    }
}
