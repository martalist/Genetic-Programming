#ifndef ProgramSettings_H
#define ProgramSettings_H

#include <vector>
#include "model/FunctionFactory.h"

/**
 * This file replicates settings that would be user configurable
 * Ideally this would be provided by XML or other file format
 * For now, it suffices to manually code configuration here
 */

namespace Model
{
    const std::vector<FunctionType> AllowedSets
    {
        FunctionType::Addition,
        FunctionType::Subtraction,
        FunctionType::Multiplication,
        FunctionType::Division,
        FunctionType::SquareRoot
    };

    const std::vector<double> Variables {0.0, 2};

    std::vector<std::vector<double>> FitnessCases
    {
        { 3.0, 5.0, 5.830952 },
        { 8.0, 14.0, 16.124515 },
        { 18.0, 2.0, 18.11077 },
        { 32.0, 11.0, 33.837849 },
        { 4.0, 3.0, 5.0 },
        { 12.0, 10.0, 15.620499 },
        { 21.0, 6.0, 21.84033 },
        { 7.0, 4.0, 8.062258 },
        { 16.0, 24.0, 28.84441 },
        { 2.0, 9.0, 9219545 }
    };
}

#endif
