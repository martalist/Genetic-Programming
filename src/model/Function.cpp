#include "Function.h"

#include <cassert>
#include <numeric>
#include <sstream>
#include <stdexcept>

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

    bool Function::SwapWith(int thisIndex, int otherIndex, std::unique_ptr<INode>& other)
    {
        // TODO: check for indices == 0 (i.e. the root node of either tree)
        auto& thisINodePtr = GetINodePointerByIndex(thisIndex);
        auto& otherINodePtr = reinterpret_cast<Function*>(other.get())->GetINodePointerByIndex(otherIndex);
        thisINodePtr.swap(otherINodePtr);
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

    std::unique_ptr<INode>& Function::GetINodePointerByIndex(int index)
    {
        assert(index > 0); // we want the pointer to the index, not the object itself
        for (auto i = 0u; i < m_children.size(); i++)
        {
            int size = m_children[i]->Size();
            if (index >= size)
            {
                // target is not in this subtree; move on
                index -= size;
                continue;
            }

            // target is in this subtree
            if (index == 0)
            {
                // it's the direct descendant
                return m_children[i];
            }
               
            // else it's deeper in the subtree
            return reinterpret_cast<Function*>(m_children[i].get())->GetINodePointerByIndex(index);
        }
        throw std::out_of_range("Index out of range in Function::GetINodePointerByIndex");
    }
}
