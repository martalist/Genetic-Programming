#include "IChromosome.h"

#include "FunctionFactory.h"

namespace Model
{
    IChromosome::IChromosome(Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>>& rand)
        : m_randInt(rand)
    {
    }
    
    bool IChromosome::IsTerminal(const std::unique_ptr<INode>& gene)
    {
        return gene->MaxChildren() == 0;
    }

    int IChromosome::RandomIndex(size_t size)
    {
        int maxIndex = static_cast<int>(size-1);
        return m_randInt.GetInRange(0, maxIndex);
    }

    void IChromosome::FillFunction(Model::INode* func, const std::vector<double*>& variables)
    {
        while (func->LacksBreadth())
        {
            auto index = RandomIndex(variables.size());
            func->AddChild(FunctionFactory::Create(variables[index]));
        }
    }
}
