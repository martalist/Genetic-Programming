#include "TerminalSymbolManager.h"

namespace Model
{
    const std::string TerminalSymbolManager::GetSymbolFor(const double* terminal)
    {
        if (m_symbols.find(terminal) != m_symbols.end())
        {
            return std::string({ m_symbols[terminal], '\0' });
        }
        m_symbols[terminal] = m_letter;
        return std::string({m_letter++, '\0'});
    }
}
