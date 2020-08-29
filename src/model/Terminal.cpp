#include "Terminal.h"

#include <stdexcept>

namespace Model
{
    Terminal::Terminal(const double* variable)
        : m_variable(variable)
    {
    }

    Terminal::Terminal(const Terminal& other)
        : m_variable(other.m_variable)
    {
    }

    double Terminal::Evaluate() const
    {
        return *m_variable;
    }

    std::string Terminal::ToString() const
    {
        return std::to_string(*m_variable);
    }

    bool Terminal::MoveChildrenTo(std::unique_ptr<INode>& other)
    {
        throw std::logic_error("Terminals should not be swapped directly");
    }

    bool Terminal::AddChild(std::unique_ptr<INode> child)
    {
        throw std::logic_error("Terminals cannot have children");
    }

    int Terminal::NumberOfChildren() const 
    {
        return 0;
    }

    int Terminal::MaxChildren() const
    {
        return 0;
    }

    int Terminal::Size() const 
    {
        return 1;
    }

    bool Terminal::IsTerminal() const
    {
        return true;
    }

    std::unique_ptr<INode>& Terminal::Get(int index, std::unique_ptr<INode>& ptr)
    {
        if (index != 0)
        {
            throw std::out_of_range("The unique ptr for a Terminal should be obtained from a Function.");
        }
        return ptr;
    }

    std::unique_ptr<INode> Terminal::Clone() const
    {
        return std::make_unique<Terminal>(*this);
    }
}
