#include <fstream>
#include <iostream>

#include "include/Graph.hpp"

int main(int argc, char **argv)
{
    if ((argc - 1) % 2 != 0)
    {
        std::cout << "An argument is missing!\n";
        return -1;
    }

    std::map<std::string, std::string> arguments;
    for (int i = 1; i < argc; ++ ++i)
    {
        arguments[std::string(argv[i])] = std::string(argv[i + 1]);
    }

    /*
     * -history json file to store history in
     * -transactions list of input transactions
     * -output json file to store final graph output in
     */

    if (arguments.contains("-history") && !arguments["-history"].ends_with(".json"))
    {
        std::cout << "History file must be a json file!\n";
        return -2;
    }
    if (arguments.contains("-output") && !arguments["-output"].ends_with(".json"))
    {
        std::cout << "Output file must be a json file!\n";
        return -3;
    }
    if (arguments.contains("-image") && !arguments["-image"].ends_with(".json"))
    {
        std::cout << "Image file must be a json file!\n";
        return -4;
    }
    if (!arguments.contains("-transactions"))
    {
        std::cout << "No transactions provided!\n";
        return -5;
    }

    std::vector<std::string> transactions;
    const std::string transactions_str = arguments["-transactions"];
    for (size_t offset = 0; offset < transactions_str.length();)
    {
        const size_t pos = transactions_str.find_first_of(' ', offset);

        if (pos == std::string::npos)
            break;

        std::string temp;
        for (size_t i = offset; i < pos; ++i)
        {
            temp += transactions_str[i];
        }

        transactions.push_back(temp);

        offset = pos + 1;
    }

    Graph graph;

    // Set up graph from image file if specified
    if (arguments.contains("-image"))
    {
        std::cout << "Image file specified, setting up graph from image!\n";
        std::fstream fstream(arguments["-image"]);
        if (!fstream.is_open())
            return -6;
        graph = nlohmann::json::parse(fstream).get<Graph>();
    }

    // Process all transactions
    for (const std::string &transaction : transactions)
        graph.processTransaction(transaction);

    // Process the CLM after processing all transactions
    graph.processCLM();

    // Output history to file
    if (arguments.contains("-history"))
    {
        std::ofstream fstream(arguments["-history"]);
        if (!fstream.is_open())
            return -7;
        fstream << nlohmann::json(graph)["jsonHistory"];
        fstream.flush();
        fstream.close();
    }

    // Output graph as JSON to file
    if (arguments.contains("-output"))
    {
        std::ofstream fstream(arguments["-output"]);
        if (!fstream.is_open())
            return -8;

        fstream << nlohmann::json(graph);
        fstream.flush();
        fstream.close();
    }

    if (!(arguments.contains("-history") && arguments.contains("-output")))
    {
        std::cout << nlohmann::json(graph).dump(2);
    }

    return 0;
}
