#include <iostream> 
#include "utils/UniformRandomGenerator.h"
#include "Program.h"

int main() 
{
    Util::UniformRandomGenerator<double> randDouble(0.0, 0.1);
    Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>, std::default_random_engine> randInteger(-100, 100);
    randInteger.SetSeed(0);

    for (int i = 0; i < 10; ++i)
    {
        if (i % 2 == 0)
        {
            std::cout << randDouble.Get() << std::endl;
        }
        else
        {
            std::cout << randInteger.Get() << std::endl;
        }
    }
    
    std::cout << "\n" << randDouble.GetInRange(10.0, 10.2) << std::endl;
    std::cout << randInteger.GetInRange(1000, 2000) << std::endl;

    Model::Program p;
    p.Start();
    
    return 0;
}
