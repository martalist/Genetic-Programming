#ifndef ISelector_H
#define ISelector_H

#include <random>

namespace Util
{
    /**
     * An interface for random selection of elements within a collection.
     */
    template <typename T>
    class ISelector
    {
    public:
        /**
         * Constructor
         */
        ISelector() {}
        ISelector(int populationSize, int tournamentSize) {}
        virtual ~ISelector() = default;

        /**
         * Registers a new element with the selector.
         * @param quantity A quantifier or weight for the element added
         * @param id The identifier of the element
         */
        virtual void RegisterElement(T quantity, int id) = 0;

        /**
         * Draw a winner from the registered elements. Assume an element of randomness.
         * @return the id of the winner, or -1 if there are no tickets
         */
        virtual int Draw() = 0;

        /**
         * Destroys the state, and starts over.
         */
        virtual void Reset() = 0;

        /**
         * @return the number of tickets issued for the raffle
         */
        virtual int NumberOfTicketsIssued() const = 0;

        /**
         * Sets the seed for the random number generator
         */
        virtual void SetSeed(int seed) = 0;
    };
}

#endif
