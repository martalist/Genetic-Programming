#include "Program.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include "ConfigParser.h"
#include "model/FunctionFactory.h"

namespace Model
{
    Program::Program()
    {
        // Get user configuration
        m_config = ConfigParser::Load("config.xml");
        m_iterations = m_config.Iterations;
        m_numGenerations = m_config.NumGenerations;

        // Create random/initial population from params
        m_population = std::make_unique<Population>(m_config.Params, m_config.FitnessCases);
    }

    Program::Program(const Config& config)
        : m_config(config)
        , m_population(std::make_unique<Population>(config.Params, config.FitnessCases))
        , m_numGenerations(config.NumGenerations)
        , m_iterations(config.Iterations)
    {
    }

    void Program::Start(bool logResults)
    {
        // Run the experiment m_iterations times
        for (int iteration = 0; iteration < m_iterations; ++iteration)
        {
            m_population->Reset(); // start with a fresh population
            
            double minimum;
            if (logResults)
            {
                m_logger.SetFilename("output" + std::to_string(iteration));
                // m_logger.WriteHeader("Average Fitness", "Best Fitness", "Best S-Expression"); // TODO

                auto [ min, firstQtr, median, thirdQtr, max ] = m_population->GetRangeStatistics();
                m_logger.AddLine(min, firstQtr, median, thirdQtr, max, "\"" + m_population->GetBestFit()->ToString() + "\"");
            }

            // evolve over m_numGenerations
            for (auto i = 0; i < m_numGenerations; ++i)
            {
                m_population->Evolve();

                if (logResults)
                {
                    auto [ min, firstQtr, median, thirdQtr, max ] = m_population->GetRangeStatistics();
                    minimum = min;

                    m_logger.AddLine(minimum, firstQtr, median, thirdQtr, max, "\"" + m_population->GetBestFit()->ToString() + "\"");
                }
                else
                {
                    minimum = m_population->GetBestFit()->Fitness();
                }

                if (minimum < m_config.StoppingCriteria)
                {
                    break;
                }
            }

            if (logResults)
            {
                // print best result
                std::cout << std::fixed << "Best S-expression in iteration " << iteration+1
                    << " has fitness: " << minimum << std::endl
                    << "\t" << m_population->GetBestFit()->ToString() << std::endl << std::endl;
            }
        }

        if (logResults)
        {
            m_logger.Write();
            std::cout << "Results written to " << m_logger.GetOutputDir() << std::endl << std::endl;
        }
    }

    double Program::Forecast(double* predictions, int length)
    {
        m_population->Forecast(predictions, length);
        return m_population->GetBestFit()->Fitness();
    }

    double Program::Predict(std::vector<double>& fitted, int cutoff)
    {
        m_population->Predict(fitted, cutoff);
        return m_population->GetBestFit()->Fitness();
    }

    double Program::Predict(const std::string& filename)
    {
        std::vector<double> result(m_config.FitnessCases);
        result.resize(m_config.FitnessCases.size() + m_config.ForecastSteps, 0.0); // append zeros
        double stdErr = Predict(result, m_config.ForecastSteps);

        std::ofstream out(filename);
        out << "actual,lower95,lower80,prediction,upper80,upper95" << std::endl;
        for (auto i = 0u; i < result.size(); ++i)
        {
            auto rooth = std::sqrt(i + 1 - m_config.FitnessCases.size());
            out << (i < m_config.FitnessCases.size() ? std::to_string(m_config.FitnessCases[i]) : "") << ",";
            out << (i >= m_config.FitnessCases.size() ? std::to_string(result[i]-1.96*stdErr*rooth) : "") << ","; 
            out << (i >= m_config.FitnessCases.size() ? std::to_string(result[i]-1.28*stdErr*rooth) : "") << ","; 
            out << (i >= m_config.Params.NumberOfTerminals ? std::to_string(result[i]) : "") << ",";
            out << (i >= m_config.FitnessCases.size() ? std::to_string(result[i]+1.28*stdErr*rooth) : "") << ","; 
            out << (i >= m_config.FitnessCases.size() ? std::to_string(result[i]+1.96*stdErr*rooth) : "") << std::endl;
        }
        out.close();
        return stdErr;
    }
}
