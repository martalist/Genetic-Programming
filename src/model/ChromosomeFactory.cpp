#include "ChromosomeFactory.h"

#include <iostream>

#include "Chromosome.h"
#include "TimeSeriesChromosome.h"

namespace Model
{
    std::unique_ptr<ChromosomeFactory> ChromosomeFactory::s_instance = nullptr;

    void ChromosomeFactory::Initialise(ChromosomeType type, int targetSize, 
            const std::vector<FunctionType>& allowedFunctions, 
            const std::vector<double*>& variables, 
            const std::vector<std::vector<double>>& fitnessCases, 
            std::vector<double>& terminals)
    {
        if (s_instance == nullptr)
        {
            auto temp = std::unique_ptr<ChromosomeFactory>(new ChromosomeFactory(type, targetSize, 
                        allowedFunctions, variables, fitnessCases, terminals));
            s_instance = std::move(temp);
        }
        else 
        {
            std::cout << "ChromosomeFactory should only be initialized once!" << std::endl;
        }
    }

    ChromosomeFactory::ChromosomeFactory(ChromosomeType type, int targetSize, 
            const std::vector<FunctionType>& allowedFunctions, 
            const std::vector<double*>& variables, 
            const std::vector<std::vector<double>>& fitnessCases, 
            std::vector<double>& terminals)
        : m_type(type)
        , m_targetSize(targetSize)
        , m_allowedFunctions(allowedFunctions)
        , m_variables(variables)
        , m_fitnessCases(fitnessCases)
        , m_terminals(terminals)
    {
    }

    const ChromosomeFactory& ChromosomeFactory::Inst()
    {
        return *s_instance;
    }


    std::unique_ptr<IChromosome> ChromosomeFactory::CreateRandom(double parsimonyCoefficient) const
    {
        switch (m_type)
        {
        case ChromosomeType::TimeSeries:
            return std::make_unique<TimeSeriesChromosome>(m_targetSize, m_allowedFunctions, m_variables, 
                    m_fitnessCases, m_terminals, parsimonyCoefficient);

        case ChromosomeType::Normal:
        default:
            return std::make_unique<Chromosome>(m_targetSize, m_allowedFunctions, m_variables, 
                    m_fitnessCases, m_terminals, parsimonyCoefficient);
        }
    }

    std::unique_ptr<IChromosome> ChromosomeFactory::CopyAndEvaluate(std::unique_ptr<INode> tree, double parsimonyCoefficient) const
    {
        switch (m_type)
        {
        case ChromosomeType::TimeSeries:
            return std::make_unique<TimeSeriesChromosome>(std::move(tree), m_fitnessCases, m_terminals, parsimonyCoefficient);

        case ChromosomeType::Normal:
        default:
            return std::make_unique<Chromosome>(std::move(tree), m_fitnessCases, m_terminals, parsimonyCoefficient);
        }
    }
}
