#ifndef UniformRandomGenerator_H
#define UniformRandomGenerator_H

#include <random>

namespace Util
{
    /**
     * A random number generator with a uniform distribution (defaults to uniform real)
     */
    template <typename T, typename UniformDistribution = std::uniform_real_distribution<T>, typename Engine = std::mt19937>
    class UniformRandomGenerator
    {
    public:
        /**
         * Constructor
         * @param low The lower bound, or minimum number that may be generated.
         * @param high The higher bound, or maximum number that may be generated.
         */
        UniformRandomGenerator(const T low, const T high) 
            : m_dist(low, high)
        {
            std::random_device rd;
            m_engine = Engine(rd());
        }

        /**
         * Sets the seed of the generator, to enable deterministic behaviour
         */
        void SetSeed(int seed)
        {
            m_engine.seed(seed);
        }

        /**
         * @return a number within the range specified by the constructor
         */
        T Get() 
        {
            return m_dist(m_engine);
        }

        /**
         * Alters the range of the generator, and fetches a number in the new range.
         * @param low The lower bound, or minimum number that may be generated.
         * @param high The higher bound, or maximum number that may be generated.
         * @return a number within the range specified.
         */
        T GetInRange(const T low, const T high)
        {
            m_dist = UniformDistribution(low, high);
            return Get();
        }
        
    private:
        UniformDistribution m_dist; ///< The the statistical distribution 
        Engine m_engine; ///< The generation engine
    };
}

#endif 
