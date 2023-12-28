#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <sstream>
#include <string>
#include "json.hpp"

class Graph {
    std::list<std::string> transactions;
    std::list<std::string> jsonHistory;
    std::list<std::pair<char, int>> nodes;
    std::list<std::tuple<char, char, std::list<char>, int>> rawEdges;
    std::vector<std::vector<int>> CLM;

    /**
     * \brief Checks if the edge exists, if so, increments the weight
     * \param fromNode The from node
     * \param toNode The to node
     * \param extraNodes The extra nodes
     * \return True if the weight was incremented otherwise false if edge was not found
     */
    bool incrementIfRawEdgeExists(const char fromNode, const char toNode, const std::list<char>& extraNodes) noexcept
    {
        for (auto& [existingFromNode, existingToNode, existingExtraNodes, existingWeight] : rawEdges)
        {
            if (existingFromNode == fromNode &&
                existingToNode == toNode &&
                existingExtraNodes == extraNodes)
            {
                ++existingWeight;
                return true;
            }
        }
        return false;
    }

    // Function to generate variations
    void generateVariations(std::list<std::string>& list, const std::string& str, std::string& current, const int index) {
        list.push_back(current);

        // Recursively generate variations
        for (int i = index; i < str.length(); ++i) {
            current.push_back(str[i]);
            generateVariations(list, str, current, i + 1);
            current.pop_back();
        }
    }

public:
    void processTransaction(const std::string& str)
    {
        // Save the transaction
        transactions.push_back(str);

        // Process nodes
        for (char it : str)
        {
            bool found = false;
            for (auto& [fst, second] : nodes)
            {
                if (fst == it)
                {
                    found = true;
                    second++;
                }
            }

            if (!found)
            {
                nodes.emplace_back(it, 1);
            }
        }
        nodes.sort();

        // Set up CLM
        const int size_y = getNodesCount();
        const int size_x = size_y * size_y + size_y;
        CLM.resize(size_y);
        for (auto& row : CLM)
        {
            row.clear();
            row.resize(size_x, 0);
        }

        // Process edges
        for (int startIndex = 0; startIndex < (str.length() - 1); startIndex++)
        {
            char fromNode = str[startIndex];

             for (int offsetIndex = startIndex + 1; offsetIndex < str.length(); ++offsetIndex)
             {
                 char toNode = str[offsetIndex];

                 std::list<char> extraNodes;
                 for (int i = offsetIndex + 1; i < str.length(); ++i)
                 {
                     extraNodes.push_back(str[i]);
                 }

                 if (!incrementIfRawEdgeExists(fromNode, toNode, extraNodes))
                 {
                     rawEdges.emplace_back(fromNode, toNode, extraNodes, 1);
                 }
             }
        }

        // Save the history
        jsonHistory.push_back(toJSON());
    }

    void processCLM()
    {
        std::map<std::string, int> itemsets;

        std::list<std::string> variations;
        std::string _;
        for (const auto& str : transactions)
        {
            generateVariations(variations, str, _, 0);
        }

        for (const auto& variant : variations)
        {
            if (variant.empty()) continue;
            itemsets[variant]++;
        }

        for (const auto& [item, frequency] : itemsets)
        {
            if (item.length() == 1)
            {
                const int pos_y = mapNodeToPosition(item[0]);
                const int pos_x = pos_y + pos_y * getNodesCount();
                CLM[pos_y][pos_x] = frequency;
            } else if (item.length() == 2)
            {
                const int pos_y = mapNodeToPosition(item[0]);
                const int pos_x = mapNodeToPosition(item[1]) + mapNodeToPosition(item[1]) * getNodesCount();

                CLM[pos_y][pos_x] = frequency;
            } else
            {
                const char fromNode = item[0];
                const char toNode = item[1];

                const char lastNode = item.back();

                const int pos_y = mapNodeToPosition(fromNode);
                const int pos_x = mapNodeToPosition(toNode) + mapNodeToPosition(toNode) * getNodesCount() + mapNodeToPosition(lastNode) + 1;

                CLM[pos_y][pos_x] = frequency;
            }
        }
    }

    [[nodiscard]] int getNodesCount() const noexcept
    {
        return static_cast<int>(nodes.size());
    }

    [[nodiscard]] int mapNodeToPosition(const char node) const noexcept
    {
        int i = 0;
        for (auto it = nodes.cbegin(); it != nodes.cend(); ++i, ++it)
        {
            if (node == it->first)
            {
                return i;
            }
        }

        return -1;
    }

    [[nodiscard]] std::list<std::string> getJSONHistory() const noexcept
    {
        return jsonHistory;
    }

    [[nodiscard]] std::string getJSONHistoryAsJSON() const noexcept
    {
        return nlohmann::json(jsonHistory).dump(0);
    }

    [[nodiscard]] std::string toJSON() const noexcept
    {
        nlohmann::json json;

        json["transactions"] = transactions;

        std::vector<nlohmann::json> nodesJSON;
        for (const auto& [node, occurrence] : nodes)
        {
            nlohmann::json nodeJSON;

            nodeJSON["node"] = std::string(1, node);
            nodeJSON["occurrence"] = occurrence;

            nodesJSON.push_back(nodeJSON);
        }
        json["nodes"] = nodesJSON;

        std::vector<nlohmann::json> edgesJSON;
        for (const auto& [fromNode, toNode, extraNodes, weight] : rawEdges)
        {
            nlohmann::json edgeJSON;

            edgeJSON["fromNode"] = std::string(1, fromNode);
            edgeJSON["toNode"] = std::string(1, toNode);

            std::list<std::string> extraNodesStr;
            for (const auto& node : extraNodes)
            {
                extraNodesStr.emplace_back(1, node);
            }

            edgeJSON["extraNodes"] = extraNodesStr;
            edgeJSON["weight"] = weight;

            edgesJSON.push_back(edgeJSON);
        }
        json["edges"] = edgesJSON;

        nlohmann::json rowsCLMJSON;
        std::vector<std::string> headers;
        auto it = nodes.cbegin();
        for (int i = 0; i < getNodesCount(); i++)
        {
            headers.emplace_back(1, it->first);

            for (const auto& key : nodes | std::views::keys)
            {
                headers.emplace_back(1, key);
            }

            ++it;
        }
        rowsCLMJSON["header"] = headers;

        it = nodes.cbegin();
        for (const auto& row : CLM)
        {
            rowsCLMJSON[std::string(1, it->first)] = row;
            ++it;
        }
        json["CLM"] = rowsCLMJSON;

        return json.dump(0);
    }

    [[nodiscard]] std::string toString() const noexcept
    {
        std::stringstream ss;

        ss << "Nodes: \n\tnode : weight\n";
        for (const auto& [node, occurrence] : nodes)
        {
            ss << '\t' << node << " : " <<  occurrence << '\n';
        }

        ss << "Edges: \n\tfrom : to : extra information : weight\n";
        for (const auto& [fromNode, toNode, extraNodes, weight] : rawEdges)
        {
            ss << '\t' << fromNode << " : " << toNode << " : ";

            for (const auto& node : extraNodes)
            {
                ss << node << ',';
            }

            ss << " : " << weight << '\n';
        }

        ss << "CLM: \n\t  | ";
        auto it = nodes.cbegin();
        for (int i = 0; i < getNodesCount(); ++i, ++it)
        {
            ss << it->first << ' ';
            ss << "| ";
            for (const auto& key : nodes | std::views::keys)
            {
                ss << key << ' ';
            }
            ss << "| ";
        }
        ss << "\n";

        it = nodes.cbegin();

        for (int i = 0; i < getNodesCount(); ++i, ++it)
        {
            ss << "\t" << it->first << " | ";

            int j = 0;
            for (const int& key : CLM[i])
            {
                if (j == 1)
                {
                    ss << "| ";
                }

                ss << key << ' ';

                j++;
                if (j == (getNodesCount() + 1))
                {
                    ss << "| ";
                    j = 0;
                }
            }
            ss << "\n";
        }

        return ss.str();
    }
};



#endif //GRAPH_H
