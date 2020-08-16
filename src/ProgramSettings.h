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
}

#endif
