#include "ConfigParser.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

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

            auto fitnessCasesFile = tree.get("Config.FitnessCases.<xmlattr>.file", std::string("pythagorean_theorem.csv"));
            s_config.Params.NumberOfTerminals = LoadFitnessCases(fitnessCasesFile);

            std::cout << "Configuration loaded:" << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << "Failed to load " << filename << ": " << e.what() << std::endl;
            std::cout << "Continuing with default values...\n" << std::endl;
        }

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

    int ConfigParser::LoadFitnessCases(const std::string& filename)
    {
        using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
        bool isHeader = true; // flags when we're reading the CSV header row
        int columns = 0; // determines the number of columns of data

        std::string line;
        std::ifstream in(filename);
        if (!in.is_open())
        {
            throw std::invalid_argument("Could not open the Fitness Cases file specified: " + filename);
        }

        while(std::getline(in, line))
        {
            Tokenizer tok(line);

            if (isHeader)
            {
                isHeader = false;
                columns = static_cast<int>(std::distance(tok.begin(), tok.end()));
                continue;
            }

            std::vector<double> row;
            for (Tokenizer::iterator itr = tok.begin(); itr != tok.end(); ++itr)
            {
                row.push_back(boost::lexical_cast<double>(*itr));
            }
            s_config.FitnessCases.push_back(row);
        }
        in.close();
        return columns - 1; // last column is the expected value, not a terminal
    }
}
