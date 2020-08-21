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
         * @param quantity The number of tickets to buy. Can be floating point.
         * @param id The identifier of the ticket holder
         */
        void BuyTickets(T quantity, int id)
        {
            m_max += quantity;
            m_tickets.push_back({ m_max, id });
        }

        /**
         * Randomly draw a winner from the ticket holders
         * @return a number within the range specified by the constructor
         */
        int Draw()
        {
            T winner = m_randomTicket.GetInRange(static_cast<T>(0), m_max);

            // Move through the list until you go past the winner's ticket No.
            // Currenlty O(n) in the worst case. Could do binary search to improve performance
            // since it's guaranteed to be sorted; TODO
            auto current = m_tickets.begin();
            while (winner > current->first)
            {
                current++;
            }
            return (--current)->second; // go back one to find the winner
        }

        /**
         * Destroys all tickets, and starts over.
         */
        void Reset()
        {
            m_tickets.clear();
        }

    private:
        UniformRandomGenerator<T, Distribution> m_randomTicket; ///< Used to randomly draw a ticket
        std::vector<std::pair<T, int>> m_tickets; ///< A list of the highest ticket held by per identifier
        T m_max = static_cast<T>(0); ///< the highest ticket held overall
    };
}

#endif
