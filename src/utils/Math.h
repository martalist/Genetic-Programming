#ifndef Math_H
#define Math_H

#include <cmath>
#include <functional>

namespace Util
{
    template <class Iterator>
    double Average(const Iterator& first, const Iterator& last, std::function<double(Iterator&)> value)
    {
        auto n = std::distance(first, last);
        double sum = 0.0;
        for (auto current = first; current != last; ++current)
        {
            sum += value(current);
        }
        return sum / n;
    }

    template <class Iterator>
    double Variance(const Iterator& first, const Iterator& last, std::function<double(const Iterator&)> value)
    {
        auto mean = Average<Iterator>(first, last, value);
        auto n = std::distance(first, last);
        double sum = 0.0;
        for (auto current = first; current != last; ++current)
        {
            sum += std::pow(value(current) - mean, 2);
        }
        return sum / (n-1);
    }

    template <class Iterator1, class Iterator2>
    double Covariance(const Iterator1& first1, const Iterator1& last1, std::function<double(Iterator1)> value1,
                      const Iterator2& first2, const Iterator2& last2, std::function<double(Iterator2)> value2)
    {
        auto n1 = std::distance(first1, last1);
        auto n2 = std::distance(first2, last2);
        if (n1 != n2)
        {
            throw std::invalid_argument("Covariance requires equal length random variables.");
        }
        auto mean1 = Average<Iterator1>(first1, last1, value1);
        auto mean2 = Average<Iterator2>(first2, last2, value2);

        double sum = 0.0;
        auto current2 = first2;
        for (auto current1 = first1; current1 != last1; ++current1, ++current2)
        {
            // TODO: should this be abs values?
            sum += (value1(current1) - mean1) * (value2(current2) - mean2);
        }
        return sum / (n1-1);
    }
}

#endif
