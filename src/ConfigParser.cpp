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
            s_config.Iterations = tree.get("Config.Iterations", 1);
            s_config.NumGenerations = tree.get("Config.Generations", 20);
            auto fitnessCasesFile = tree.get("Config.FitnessCases.<xmlattr>.file", std::string("FitnessCases.csv"));
            // TODO: not sure if this should be in config, or just fetched from the fitnessCasesFile
            // s_config.Params.NumberOfTerminals = tree.get("Config.FitnessCases.<xmlattr>.numTerminals", 2);
            s_config.Params.PopulationSize = tree.get("Config.Population.Size", 1000);
            s_config.Params.CrossoverProb = tree.get<double>("Config.CrossoverProb", 0.7);
            s_config.Params.MutationProb = tree.get<double>("Config.MutationProb", 0.001);
            s_config.Params.MinInitialTreeSize = tree.get("Config.Population.MinInitTreeSize", 10);
            auto optSeed = tree.get_optional<int>("Config.Seed");
            if (optSeed)
            {
                s_config.Params.Seed = *optSeed;
            }
            for (const auto& func : tree.get_child("Config.AllowedFunctions"))
            {
                auto funcType = FunctionFactory::AsFunctionType(func.second.data());
                s_config.Params.AllowedFunctions.push_back(funcType);
            }
            std::cout << "Configuration loaded:" << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << "Failed to load " << filename << ": " << e.what() << std::endl;
            std::cout << "Continuing with default values...\n" << std::endl;
        }

        // TODO: load fitness cases from CSV file

        // TODO: @see note above
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
