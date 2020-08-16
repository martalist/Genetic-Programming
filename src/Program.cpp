#include "Program.h"

#include <iostream>
#include "ProgramSettings.h"
#include "model/INode.h"
#include "model/FunctionFactory.h"
#include "model/Variable.h"

namespace Model
{

    Util::UniformRandomGenerator<int, std::uniform_int_distribution<int>> Program::s_randInt(0, 1);
    Program::Program()
        : m_allowedFunctions(AllowedSets)
    {
        // parse command line input

        // m_population.reserve(m_populationSize); // make room
    }

    void Program::TemporaryTesting()
    {
        double a = 1.0;
        double b = 2.2;

        auto aVar = std::make_unique<Variable>(a);
        auto bVar = std::make_unique<Variable>(b);
        std::cout << "a = " << a << " = " << aVar->Evaluate() << std::endl;
        std::cout << "b = " << b << " = " << bVar->Evaluate() << std::endl;
        
        // test that the reference works
        b = 2.5;
        std::cout << "b = " << b << " = " << bVar->Evaluate() << std::endl;

        auto add = FunctionFactory::Create(FunctionType::Addition);
        add->AddChild(std::make_unique<Variable>(a));
        add->AddChild(std::make_unique<Variable>(b));
        std::cout << "a + b = " << a << " + " << b << " = " 
            << add->Evaluate() << std::endl;

        auto sub = FunctionFactory::Create(FunctionType::Subtraction);
        sub->AddChild(std::make_unique<Variable>(a));
        sub->AddChild(std::make_unique<Variable>(b));
        std::cout << "a - b = " << sub->Evaluate() << std::endl;

        auto mult = FunctionFactory::Create(FunctionType::Multiplication);
        mult->AddChild(std::make_unique<Variable>(a));
        mult->AddChild(std::make_unique<Variable>(b));
        std::cout << "a * b = " << a << " * " << b << " = " 
            << mult->Evaluate() << std::endl;

        auto div = FunctionFactory::Create(FunctionType::Division);
        div->AddChild(std::make_unique<Variable>(a));
        div->AddChild(std::make_unique<Variable>(b));
        std::cout << "a / b = " << a << " / " << b << " = " 
            << div->Evaluate() << std::endl;

        // auto sq = FunctionFactory::Create(FunctionType::SquareRoot);
        // sq->AddChild(std::move(bVar));
        // std::cout << "sqrt(b) = " << sq->Evaluate() << std::endl;

        auto nested = FunctionFactory::Create(FunctionType::SquareRoot);
        auto squared = FunctionFactory::Create(FunctionType::Multiplication);
        squared->AddChild(std::make_unique<Variable>(b));
        squared->AddChild(std::make_unique<Variable>(b));
        squared->AddChild(std::make_unique<Variable>(b));
        squared->AddChild(std::make_unique<Variable>(b));
        nested->AddChild(std::move(squared));
        std::cout << "sqrt(b*b) = " << nested->Evaluate() << std::endl;
    }
    
    void Program::Start()
    {
        TemporaryTesting();
        // generate m_populationSize chromosomes
        for (int i = 0u; i < m_populationSize; ++i)
        {
            // m_population.push_back(std::move(CreateRandomChromosome()));
        }

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

    std::unique_ptr<INode> Program::CreateRandomChromosome() const
    {
        // start with a randomly selected function
        int index = RandomIndex(m_allowedFunctions.size());
        auto root = FunctionFactory::Create(m_allowedFunctions[index]);

        // keep a local list of function nodes, for easy reference
        std::vector<INode*> functions;
        functions.push_back(root.get());
        int count = 1; // easier to do account keeping this way, than in the tree
        while (count < m_minInitialTreeSize)
        {
            // Randomly choose a new function/variable
            auto newNode = FunctionFactory::Create(FunctionType::Addition);

            // add the new node to the randomly selected one
            auto func = functions[ RandomIndex(functions.size()) ];
            func->AddChild(std::move(newNode));
        }
        
        // Make sure none of the leaf nodes are functions
        for (auto& func : functions)
        {
            if (func->NumberOfChildren() == 0)
            {
                // add a variable
            }
        }
        return root;
    }
    
    int Program::RandomIndex(size_t size) const
    {
        int maxIndex = static_cast<int>(--size);
        return s_randInt.GetInRange(0, maxIndex);
    }
}
