#pragma once

namespace Model
{
    /**
     * Types of Chromosome
     */
    enum class ChromosomeType
    {
        Normal = 0, ///< general purpose chromosomes for fitting 
        TimeSeries  ///< auto-regressive, non-linear, with least-squares-fit coefficients
    };
}
