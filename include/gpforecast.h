#ifndef GPFORECAST_H
#define GPFORECAST_H

#include <cstdint>

extern "C"
{
    typedef struct _settings
    {
        int32_t lag;
        int32_t seed; // a positive integer yeilds deterministic results. If -1, no seed is set.
        double stopping_criteria; 
        uint8_t log_results; // if true, the GP will output a csv for each prediction made
    } Settings;

    typedef struct _timeSeriesData
    {
        double* data;
        int32_t length;
    } TimeSeriesData;

    double PredictTimeSeries(Settings* settings, TimeSeriesData* history, TimeSeriesData* prediction);
}

#endif
