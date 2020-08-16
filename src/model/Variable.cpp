#include "Variable.h"

namespace Model
{
    Variable::Variable(const double& variable)
        : m_variable(variable)
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

    bool Variable::SwapWith(std::unique_ptr<INode> child)
    {
        return true;
    }

    bool Variable::AddChild(std::unique_ptr<INode> child)
    {
        throw std::exception();
    }

    int Variable::NumberOfChildren() const 
    {
        return 0;
    }
}
