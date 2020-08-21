#include "Variable.h"

#include <stdexcept>

namespace Model
{
    Variable::Variable(const double& variable)
        : m_variable(variable)
    {
    }

    Variable::Variable(const Variable& other)
        : m_variable(other.m_variable)
    {
    }

    double Variable::Evaluate() const
    {
        return m_variable;
    }

    std::string Variable::ToString() const
    {
        return std::to_string(m_variable);
    }

    bool Variable::SwapWith(std::unique_ptr<INode> other)
    {
        throw std::logic_error("Variables should not be swapped directly");
    }

    bool Variable::AddChild(std::unique_ptr<INode> child)
    {
        throw std::logic_error("Variables cannot have children");
    }

    int Variable::NumberOfChildren() const 
    {
        return 0;
    }

    int Variable::MaxChildren() const
    {
        return 0;
    }

    int Variable::Size() const 
    {
        return 1;
    }

    bool Variable::IsVariable() const
    {
        return true;
    }

    std::unique_ptr<INode>& Variable::Get(int index)
    {
        throw std::out_of_range("The unique ptr for a Variable should be obtained from a Function.");
    }
}
