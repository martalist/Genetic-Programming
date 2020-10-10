#ifndef Raffle_H
#define Raffle_H

#include <utility>
#include <vector>
#include "UniformRandomGenerator.h"

namespace Util
{
    /**
     * A raffle ticketing system, allowing sequential allocation of tickets, and 
     * randomly drawing winners.
     * Note that the winning ticket is not removed from the hat, so could be drawn
     * a second time.
     * Ticket numbers may be floating-point/continuous in this simulated raffle.
     */
    template <typename T, typename Distribution = std::uniform_real_distribution<T>>
    class Raffle
    {
    public:
        /**
         * Constructor
         */
        Raffle() 
            : m_randomTicket(static_cast<T>(0), static_cast<T>(1)) { }

        /**
         * Allocates the spectified quantity of raffle tickets to an entity.
         * @param quantity The number of tickets to buy. Must be >= 0. Can be floating point.
         * @param id The identifier of the ticket holder
         */
        void RegisterElement(T quantity, int id)
        {
            if (quantity == static_cast<T>(0))
            {
                return;
            }
            m_max += quantity;
            m_tickets.push_back({ m_max, id });
        }

        /**
         * Randomly draw a winner from the ticket holders
         * @return the id of the winner, or -1 if there are no tickets
         */
        int Draw()
        {
            if (m_tickets.size() == 0)
            {
                return -1;
            }
            T winningTicket = m_randomTicket.GetInRange(static_cast<T>(0), m_max);

            // Move through the list until you go past the winner's ticket No.
            // Currenlty O(n) in the worst case. Could do binary search to improve performance
            // since it's guaranteed to be sorted; TODO
            auto current = m_tickets.begin();
            while (winningTicket > current->first)
            {
                current++;
            }
            return current->second; // go back one to find the winner
        }

        /**
         * Destroys all tickets, and starts over.
         */
        void Reset()
        {
            m_tickets.clear();
        }

        /**
         * @return the number of tickets issued for the raffle
         */
        int NumberOfTicketsIssued() const
        {
            return static_cast<int>(m_tickets.size());
        }

        /**
         * Sets the seed for the random number generator
         */
        void SetSeed(int seed)
        {
            m_randomTicket.SetSeed(seed);
        }

    private:
        UniformRandomGenerator<T, Distribution> m_randomTicket; ///< Used to randomly draw a ticket
        std::vector<std::pair<T, int>> m_tickets; ///< A list of the highest ticket held by per identifier
        T m_max = static_cast<T>(0); ///< the highest ticket held overall
    };
}

#endif
