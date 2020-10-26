#include "ChromosomeUtil.h"

#include "INode.h"
#include "FunctionFactory.h"

namespace Model::ChromosomeUtil
{
    Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>>& RandInt()
    {
        static Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>> randInt(0, 1);
        return randInt;
    }

    void SetSeed(int seed)
    {
        RandInt().SetSeed(seed);
    }

    bool IsTerminal(const std::unique_ptr<INode>& gene)
    {
        return gene->MaxChildren() == 0;
    }

    int RandomIndex(size_t size)
    {
        int maxIndex = static_cast<int>(size-1);
        return RandInt().GetInRange(0, maxIndex);
    }

    void FillFunction(Model::INode* func, const std::vector<double*>& variables)
    {
        while (func->LacksBreadth())
        {
            auto index = RandomIndex(variables.size());
            func->AddChild(FunctionFactory::Create(variables[index]));
        }
    }
}
