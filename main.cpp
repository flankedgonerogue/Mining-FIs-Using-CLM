#include <fstream>
#include <iostream>

#include "include/Graph.hpp"

int main(int argc, char **argv)
{
    // Check if arguments are in pairs
    if ((argc - 1) % 2 != 0)
    {
        std::cout << "An argument is missing!\n";
        return 1;
    }

    // Get all arguments in a map
    std::map<std::string, std::string> arguments;
    for (int i = 1; i < argc; ++ ++i)
    {
        arguments[std::string(argv[i])] = std::string(argv[i + 1]);
    }

    // Check for valid max nodes
    int maxNodes;
    if (arguments.contains("-max-nodes"))
    {
        try
        {
            maxNodes = std::stoi(arguments["-max-nodes"]);

            if (maxNodes < 1)
            {
                throw std::exception();
            }
        } catch (std::exception& _)
        {
            std::cout << "Max nodes is not a valid number!\n";
            std::cout << _.what();

            return 2;
        }
    } else
    {
        std::cout << "Max nodes are not provided!\n";
        return 3;
    }

    // Check for valid min support
    int minSupport;
    if (arguments.contains("-min-support"))
    {
        try
        {
            minSupport = std::stoi(arguments["-min-support"]);

            if (minSupport < 1)
            {
                throw std::exception();
            }

        } catch (std::exception& _)
        {
            std::cout << "Min support is not a valid number!\n";
            std::cout << _.what();

            return 4;
        }
    } else
    {
        std::cout << "Min support is not provided!\n";
        return 5;
    }

    // Check for valid transactions
    if (!arguments.contains("-transactions"))
    {
        std::cout << "No transactions provided!\n";
        return 6;
    }
    std::vector<std::string> transactions;
    const std::string transactions_str = arguments["-transactions"];
    for (size_t offset = 0; offset < transactions_str.length();)
    {
        const size_t pos = transactions_str.find_first_of(' ', offset);

        std::string temp;
        for (size_t i = offset; i < pos && i < transactions_str.length(); ++i)
        {
            temp += transactions_str[i];
        }


        transactions.push_back(temp);

        if (pos == std::string::npos) break;
        offset = pos + 1;
    }

    // Create the graph
    Graph graph(maxNodes);

    // Set up graph from image file if specified (30 - Failed Image Read)
    if (arguments.contains("-image"))
    {
        std::cout << "Image file specified, setting up graph from image!\n";
        std::fstream fstream(arguments["-image"]);
        if (!fstream.is_open())
        {
            std::cout << "Failed to read image file!";
            return 30;
        }
        graph = nlohmann::json::parse(fstream).get<Graph>();
        graph.setMaxNodes(maxNodes);
    }

    // Process all transactions
    for (const std::string &transaction : transactions)
    {
        graph.processTransaction(transaction);
    }

    // Output to Console if no output files specified
    if (!(arguments.contains("-output") && arguments.contains("-fis-output")))
    {
        const auto& FIs = graph.useCLM_Miner(minSupport);
        std::cout << graph.toString();

        std::cout << "FIs:\n";
        for (const auto& FI : FIs)
        {
            std::cout << '\t' << FI << '\n';
        }
    }

    // Output graph as JSON to JSON file (10 - Bad file, 11 - Failed Output)
    if (arguments.contains("-output"))
    {
        if (!arguments["-output"].ends_with(".json"))
        {
            std::cout << "Bad graph output file!\n";
            return 10;
        }

        std::ofstream fstream(arguments["-output"]);
        if (!fstream.is_open())
        {
            std::cout << "Failed graph output!\n";
            return 11;
        }

        fstream << nlohmann::json(graph);
        fstream.flush();
        fstream.close();
    }

    // Output FIs to a JSON file (20 - Bad file, 21 - Failed Output)
    if (arguments.contains("-fis-output"))
    {
        if (!arguments["-image"].ends_with(".json"))
        {
            std::cout << "Bad FIs output file!\n";
            return 20;
        }

        std::ofstream fstream(arguments["-fis-output"]);
        if (!fstream.is_open())
        {
            std::cout << "Failed FIs output!\n";
            return 21;
        }

        fstream << nlohmann::json(graph.useCLM_Miner(minSupport));
        fstream.flush();
        fstream.close();
    }

    return 0;
}
