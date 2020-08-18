#include "Function.h"

#include <numeric>

namespace Model
{
    Function::Function(std::function<double(const ChildNodes&)> func,
                int maxChildren /*= std::numeric_limits<int>::max()*/)
        : MaxChildren(maxChildren)
        , m_func(func)
    {
    }

    Function::Function(const Function& other)
        : MaxChildren(other.MaxChildren)
        , m_func(other.m_func)
    {
        // TODO: perform a deep copy of m_children
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
        if (static_cast<int>(m_children.size()) < MaxChildren)
        {
            m_children.push_back(std::move(child));
            return true;
        }
        return false;
    }

    bool Function::SwapWith(std::unique_ptr<INode> child)
    {
        return true;
    }

    int Function::NumberOfChildren() const 
    {
        return static_cast<int>(m_children.size());
    }

    int Function::Size() const 
    {
        return std::accumulate(m_children.begin(), m_children.end(), 1,
                [](int a, const auto& child) { return a + child->Size(); });
    }

    bool Function::IsVariable() const
    {
        return false;
    }
}
