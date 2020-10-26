#include "Program.h"

int main() 
{
    const bool enableLogging = true;
    Model::Program p;
    p.Start(enableLogging);
    p.Predict();
    
    return 0;
}
