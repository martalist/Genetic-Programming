#include "ChromosomeFactory.h"

#include <iostream>

#include "../PopulationParams.h"
#include "Chromosome.h"
#include "TimeSeriesChromosome.h"

namespace Model
{
    ChromosomeFactory::ChromosomeFactory(ChromosomeType type, int targetSize, 
            const std::vector<FunctionType>& allowedFunctions, 
            const std::vector<double*>& variables, 
            const TrainingData& fitnessCases, 
            std::vector<double>& terminals)
        : m_type(type)
        , m_targetSize(targetSize)
        , m_allowedFunctions(allowedFunctions)
        , m_variables(variables)
        , m_fitnessCases(fitnessCases)
        , m_terminals(terminals)
    {
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
