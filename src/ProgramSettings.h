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
    const std::vector<FunctionType> AllowedFunctions
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
        { 2.0, 9.0, 9.219545 }
    };

    // TODO: would be good to have this implemented here, independent of the software
    auto fitnessFunc = [](const INode& chromosome, const std::vector<std::vector<double>>& fitnessCases) -> double
    {
        // double sumOfErrors = 0.0;
        // for (auto& fCase : fitnessCases) // FitnessCases are the training set
        // {
            // for (auto i = 0u; i < fCase.size()-1; i++)
            // {
                // m_terminals[i] = fCase[i];
            // }

            // // calculate the fitness based on the variable values
            // // and expected result for each case
            // auto caseFitness = std::abs(m_population[index]->Evaluate() - fCase.back());

            // // add to the tally
            // sumOfErrors += caseFitness;
        // }
        // return sumOfErrors;
        return 0.0;
    };

    // TODO
    // auto ticketAllocation = [](double fitness) 
    // {
        // return std::exp(4.0 - std::pow(fitness, 2.0/5.0));
    // };
}

#endif
