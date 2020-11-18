#include "Terminal.h"

#include <stdexcept>

namespace Model
{
    Terminal::Terminal(const double* variable, const std::string& symbol)
        : m_variable(variable)
        , m_symbol(symbol)
    {
    }

    Terminal::Terminal(const Terminal& other)
        : m_variable(other.m_variable)
        , m_symbol(other.m_symbol)
    {
    }

    double Terminal::Evaluate() const
    {
        return *m_variable;
    }

    std::string Terminal::ToString() const
    {
        return m_symbol.empty() ? std::to_string(*m_variable) : m_symbol;
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

    const std::vector<std::unique_ptr<INode>>& Terminal::GetChildren() const
    {
        throw std::logic_error("Terminals do not have anly children.");
    }

    int Terminal::MaxChildren() const
    {
        return 0;
    }

    int Terminal::Size() const 
    {
        return 1;
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

    std::string Terminal::GetSymbol() const
    {
        return m_symbol;
    }
}
