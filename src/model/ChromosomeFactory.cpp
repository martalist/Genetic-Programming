#include "ChromosomeFactory.h"

#include <iostream>

#include "../PopulationParams.h"
#include "NormalChromosome.h"
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
        , m_randInt(0, 1)
    {
    }

    std::unique_ptr<IChromosome> ChromosomeFactory::CreateRandom(double parsimonyCoefficient) const
    {
        switch (m_type)
        {
        case ChromosomeType::TimeSeries:
            return std::make_unique<TimeSeriesChromosome>(m_targetSize, m_allowedFunctions, m_variables, 
                    m_fitnessCases, m_terminals, parsimonyCoefficient, m_randInt, m_funcFactory);

        case ChromosomeType::Normal:
        default:
            return std::make_unique<NormalChromosome>(m_targetSize, m_allowedFunctions, m_variables, 
                    m_fitnessCases, m_terminals, parsimonyCoefficient, m_randInt, m_funcFactory);
        }
    }

    std::unique_ptr<IChromosome> ChromosomeFactory::CopyAndEvaluate(std::unique_ptr<INode> tree, double parsimonyCoefficient) const
    {
        switch (m_type)
        {
        case ChromosomeType::TimeSeries:
            return std::make_unique<TimeSeriesChromosome>(std::move(tree), m_fitnessCases, m_terminals, parsimonyCoefficient, m_randInt, m_funcFactory);

        case ChromosomeType::Normal:
        default:
            return std::make_unique<NormalChromosome>(std::move(tree), m_fitnessCases, m_terminals, parsimonyCoefficient, m_randInt, m_funcFactory);
        }
    }

    void ChromosomeFactory::SetSeed(int seed)
    {
        m_randInt.SetSeed(seed);
    }
}
