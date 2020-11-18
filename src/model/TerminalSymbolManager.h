#ifndef TerminalSymbolManager_H
#define TerminalSymbolManager_H

#include <map>
#include <string>

namespace Model
{
    /**
     * A class responsible for dispensing unique symbols (characters) to represent terminals
     */
    class TerminalSymbolManager
    {
    public:
        /**
         * Constructor
         */
        TerminalSymbolManager() = default;
        ~TerminalSymbolManager() = default;

        /**
         * @param A pointer to the terminal
         * @return the next available symbol
         */
        const std::string GetSymbolFor(const double* terminal);

    private:
        char m_letter = 'A'; ///< The symbol for the next terminal
        std::map<const double*, char> m_symbols; ///< Maps symbols to the terminal pointer
    };
}
#endif
