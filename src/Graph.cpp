#include "../include/Graph.hpp"

// PRIVATE FUNCTIONS

bool Graph::incrementIfRawEdgeExists(const char fromNode, const char toNode, const std::list<char> &extraNodes) noexcept
{
    for (auto &[existingFromNode, existingToNode, existingExtraNodes, existingWeight] : edges)
    {
        if (existingFromNode == fromNode && existingToNode == toNode && existingExtraNodes == extraNodes)
        {
            ++existingWeight;
            return true;
        }
    }
    return false;
}

size_t Graph::mapNodeToPosition(const char node) const noexcept
{
    int i = 0;
    for (auto it = nodes.cbegin(); it != nodes.cend(); ++i, ++it)
    {
        if (node == it->label)
        {
            return i;
        }
    }

    return -1;
}

char Graph::mapPostionToNode(const size_t node) const noexcept
{
    auto it = nodes.cbegin();
    for (int i = 0; it != nodes.cend(); ++i, ++it)
    {
        if (node == i)
        {
            return it->label;
        }
    }

    return -1;
}


// PUBLIC FUNCTIONS

void Graph::setMaxNodes(const size_t maxNodes) noexcept
{
    this->maxNodes = maxNodes;
    this->maxRowSize = maxNodes * maxNodes + maxNodes;
}

void Graph::processTransaction(const std::string &str)
{
    // Process nodes
    for (char it : str)
    {
        bool found = false;
        for (auto &[fst, second] : nodes)
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
    for (const auto &[label, occurrence] : nodes)
    {
        if (!CLM.contains(label))
            CLM.emplace(label, std::vector<size_t>(maxRowSize, 0));
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
                edges.emplace_back(fromNode, toNode, extraNodes, 1);
            }
        }
    }
}

std::list<std::string> Graph::useCLM_Miner(const int minSup)
{
    // Process CLM
    // Update the individual nodes
    for (const auto &[label, occurrence] : nodes)
    {
        CLM[label][mapNodeToPosition(label) * (maxNodes + 1)] = occurrence;
    }
    // Update using edges
    for (const auto &[fromNode, toNode, extraNodes, occurrence] : edges)
    {
        const size_t startPosX = mapNodeToPosition(toNode) * (maxNodes + 1);
        CLM[fromNode][startPosX] += occurrence;

        for (const char &extraNode : extraNodes)
        {
            CLM[fromNode][startPosX + mapNodeToPosition(extraNode) + 1] += occurrence;
        }
    }

    // Process FIs
    std::list<std::string> FIs;

    // Iterate over each row
    for (const auto &[label, row] : CLM)
    {
        // Iterate over major columns
        for (size_t i = 0; i < maxRowSize; i += (maxNodes + 1))
        {
            // If the major column support count is greater than the minimum support count, check the minor columns
            if (row[i] >= minSup)
            {
                // Add the row and Major to MFI
                std::string temp;
                temp += label;
                const char _ = mapPostionToNode(i % maxNodes);
                if (_ == label)
                {
                    FIs.push_back(temp);
                    continue;
                }
                temp += _;

                FIs.push_back(temp);

                std::vector positions(maxNodes, false);
                // Store positions of all minor columns greater than min support
                for (size_t j = i + 1, k = 0; k < maxNodes; ++j, ++k)
                {
                    positions[k] = row[j] > minSup;
                }

                for (size_t j = 0; j < positions.size(); j++)
                {
                    if (!positions[j])
                        continue;

                    std::string _temp;
                    _temp += mapPostionToNode(j);
                    FIs.push_back(temp + _temp);
                    for (size_t k = j + 1; positions[k] && k < maxNodes; ++k)
                    {
                        _temp += mapPostionToNode(k);
                        FIs.push_back(temp + _temp);
                    }
                }
            }
        }
    }

    return FIs;
}

std::string Graph::toString() const noexcept
{
    std::stringstream ss;

    ss << "Nodes: \n\tnode : weight\n";
    for (const auto &[node, occurrence] : nodes)
    {
        ss << '\t' << node << " : " << occurrence << '\n';
    }

    ss << "Edges: \n\tfrom : to : extra information : weight\n";
    for (const auto &[fromNode, toNode, extraNodes, weight] : edges)
    {
        ss << '\t' << fromNode << " : " << toNode << " : ";

        for (const auto &node : extraNodes)
        {
            ss << node << ',';
        }

        ss << " : " << weight << '\n';
    }

    ss << "CLM: \n\t  | ";
    auto it = nodes.cbegin();
    for (int i = 0; i < maxNodes; ++i, ++it)
    {
        ss << it->label << ' ';
        ss << "| ";
        for (const auto &[label, occurrence] : nodes)
        {
            ss << label << ' ';
        }
        ss << "| ";
    }
    ss << "\n";

    it = nodes.cbegin();

    for (int i = 0; i < maxNodes; ++i, ++it)
    {
        ss << "\t" << it->label << " | ";

        int j = 0;
        for (const size_t &key : CLM.find(it->label)->second)
        {
            if (j == 1)
            {
                ss << "| ";
            }

            ss << key << ' ';

            j++;
            if (j == (maxNodes + 1))
            {
                ss << "| ";
                j = 0;
            }
        }
        ss << "\n";
    }

    return ss.str();
}
