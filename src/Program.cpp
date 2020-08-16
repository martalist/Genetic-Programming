#include "Program.h"
#include "model/INode.h"

namespace Model
{
    Program::Program()
    {
        // parse command line input

        // m_population.reserve(m_populationSize); // make room
    }

    void Program::Start()
    {
        // generate m_populationSize chromosomes
        // evaluate the fitness
        // evolve over m_numGenerations

        for (auto i = 0u; i < m_numGenerations; ++i)
        {
            std::vector<int> newPopulation;
            // newPopulation.reserve(m_populationSize); // make room 

            // breed
            while (newPopulation.size() < m_populationSize)
            {
                // select a breeding pair and (deep) copy them
                // auto& [mum, dad] = SelectParents();

                // perform crossover & mutation
                // auto& [son, daughter] = Reproduce(mum, dad);
                
                // add to new population
                newPopulation.push_back(0);
                newPopulation.push_back(0);
            }
        }
    }
}
