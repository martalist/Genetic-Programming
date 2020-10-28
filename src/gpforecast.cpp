#include "gpforecast.h"

#include <algorithm>
#include <vector>
#include "Program.h"
#include "PopulationParams.h"
#include "model/FunctionFactory.h"
#include "model/IChromosome.h"

namespace
{
    const std::vector<Model::FunctionType> DefaultAllowedFunctions
    {
        Model::FunctionType::Subtraction,
        Model::FunctionType::Multiplication,
        Model::FunctionType::Division,
        Model::FunctionType::Sine,
        Model::FunctionType::Cosine,
        Model::FunctionType::NaturalLogarithm
    };
}

double PredictTimeSeries(Settings* settings, TimeSeriesData* history, TimeSeriesData* prediction)
{
    // Set up the config
    Model::Config config;
    // config.Iterations = 1; // defaults to 1, which is what we want
    config.NumGenerations = 10;
    config.StoppingCriteria = settings->stopping_criteria;
    config.Params.Type = Model::ChromosomeType::TimeSeries;
    config.Params.PopulationSize = 500;
    config.Params.MinInitialTreeSize = 20;
    // config.Params.CrossoverProb = 0.7; // default is fine
    config.Params.MutationProb = 0.1;
    config.Params.HoistMutationProb = 0.1;
    config.Params.AllowedFunctions = DefaultAllowedFunctions;
    config.Params.NumberOfTerminals = settings->lag;
    if (settings->seed >=0)
    {
        config.Params.Seed = settings->seed;
    }

    // copy the fitness cases
    // TODO: this is not efficient. Consider refactoring so that copying is not required.
    for (int i = 0; i < history->length; ++i)
    {
        config.FitnessCases.push_back(*(history->data + i));
    }

    Model::Program p(config);

    p.Start(settings->log_results);
    return p.Forecast(prediction->data, prediction->length);
}
