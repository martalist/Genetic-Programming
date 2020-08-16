#include "Function.h"

namespace Model
{
    Function::Function(std::function<double(const ChildNodes&)> func,
                int maxChildren /*= std::numeric_limits<int>::infinity()*/)
        : MaxChildren(maxChildren)
        , m_func(func)
    {
    }

    double Function::Evaluate() const
    {
        return m_func(m_children);
    }

    std::string Function::ToString() const
    {
        return "";
    }

    bool Function::AddChild(std::unique_ptr<INode> child)
    {
        m_children.push_back(std::move(child));
        return true;
    }

    bool Function::SwapWith(std::unique_ptr<INode> child)
    {
        return true;
    }
}
