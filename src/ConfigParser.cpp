#include "ConfigParser.h"

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "model/FunctionFactory.h"

namespace Model
{
    Config ConfigParser::s_config{};

    Config ConfigParser::Load(const std::string& filename)
    {
        try
        {
            namespace pt = boost::property_tree;

            pt::ptree tree;
            pt::read_xml(filename, tree);
            // s_file = tree.get<std::string>("debug.filename");
            //
            // TODO: fetch config from XML and load up s_config
        }
        catch (std::exception& e)
        {
            std::cout << "Failed to load " << filename << ". Continuing with default values..." << std::endl;
            // do nothing; provide
        }

        s_config.Params.AllowedFunctions =  // TODO: should be loaded from XML
        {
            FunctionType::Addition,
            FunctionType::Subtraction,
            FunctionType::Multiplication,
            FunctionType::Division,
            FunctionType::SquareRoot
        };
        s_config.Params.NumberOfTerminals = static_cast<int>(s_config.FitnessCases[0].size()-1);

        PrintConfig();
        return s_config;
    }

    void ConfigParser::PrintConfig()
    {
        std::cout << "\tIterations: " << s_config.Iterations << std::endl;
        std::cout << "\tGenerations: " << s_config.NumGenerations << std::endl;
        std::cout << "\tPopulation Size: " << s_config.Params.PopulationSize << std::endl;
        std::cout << "\tMinimum initial S-expressions size: " << s_config.Params.MinInitialTreeSize << std::endl;
        std::cout << "\tCrossover probability: " << s_config.Params.CrossoverProb << std::endl;
        std::cout << "\tMutation probability: " << s_config.Params.MutationProb << std::endl;
        std::cout << "\tNumber of terminals: " << s_config.Params.NumberOfTerminals << std::endl;

        std::cout << "\tAllowed functions: ";
        int i = 0;
        auto size = s_config.Params.AllowedFunctions.size();
        for (auto& func : s_config.Params.AllowedFunctions)
        {
            std::cout << FunctionFactory::AsString(func) << (++i == size ? "\n" : ", ");
        }

        if (s_config.Params.Seed.has_value())
        {
            std::cout << "\tSeed: " << s_config.Params.Seed.value() << std::endl;
        }
        std::cout << std::endl;
    }
}
