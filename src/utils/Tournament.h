#ifndef Tournament_H
#define Tournament_H

#include "ISelector.h"
#include "UniformRandomGenerator.h"

namespace Util
{
    /**
     * A tournament selection system.
     * Represents a sorted (ascending) population of size N (by indices only).
     * To draw a "winner", randomly selects a sub-population of size M and returns
     * the lowest index (representing the smallest/best member of the population)
     *
     * @pre-condition Assumes that the population is sorted.
     */
    template <typename T, typename Distribution = std::uniform_int_distribution<int>>
    class Tournament : public ISelector<T>
    {
    public:
        /**
         * Constructor
         */
        Tournament(int populationSize, int tournamentSize)
            : m_randomIndex(0, tournamentSize) 
            , m_populationSize(populationSize)
            , m_tournamentSize(tournamentSize)
        { }

        /**
         * @see ISelector::RegisterElement
         */
        void RegisterElement(T quantity, int id)
        {
            // do nothing
        }

        /**
         * Randomly draw a winner from the ticket holders
         * @return the id of the winner, or -1 if there are no tickets
         */
        int Draw()
        {
            if (m_populationSize <= 0)
            {
                return -1;
            }

            int winner = std::numeric_limits<int>::max();

            for (int i = 0; i < m_tournamentSize; ++i)
            {
                winner = std::min(winner, m_randomIndex.Get());
            }
            return winner;
        }

        /**
         * Destroys all tickets, and starts over.
         */
        void Reset()
        {
            // do nothing
        }

        /**
         * @return the number of tickets issued for the raffle
         */
        int NumberOfTicketsIssued() const
        {
            return m_populationSize;
        }

        /**
         * Sets the seed for the random number generator
         */
        void SetSeed(int seed)
        {
            m_randomIndex.SetSeed(seed);
        }

    private:
        UniformRandomGenerator<int, Distribution> m_randomIndex; ///< Used to randomly draw a ticket
        int m_populationSize = 0; ///< Total size of the population represented
        const int m_tournamentSize = 0; ///< Size of the random sub-population
    };
}

#endif
