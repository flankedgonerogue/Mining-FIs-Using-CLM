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

            bool found = false;
            for (auto &[existingFromNode, existingToNode, existingExtraNodes, existingWeight] : edges)
            {
                if (existingFromNode == fromNode && existingToNode == toNode && existingExtraNodes == extraNodes)
                {
                    ++existingWeight;
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                edges.emplace_back(fromNode, toNode, extraNodes, 1);
            }
        }
    }
}

std::list<std::string> Graph::useCLM_Miner(const int minSup)
{
    // Set up CLM
    nodes.sort();
    for (const auto &[label, occurrence] : nodes)
    {
        if (!CLM.contains(label))
            CLM.emplace(label, std::vector<size_t>(maxRowSize, 0));
    }

    // Fill CLM
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
    for (auto &[label, row] : CLM)
    {
        // Iterate over major columns
        for (size_t i = 0; i < maxRowSize; i += (maxNodes + 1))
        {
            // If the major column support count is greater than the minimum support count, check the minor columns
            if (row[i] >= minSup)
            {
                // Add the row and major column to FIs
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

                std::string _temp;
                size_t j = maxNodes + i; // Starting from the final minor column
                while (j > i)
                {
                    if (row[j] >= minSup)
                    {
                        // Add Row + Major Col + Minor Col to FIs
                        _temp = mapPostionToNode(j - i - 1);
                        FIs.push_back(temp + _temp);

                        // TODO FIX for FIs greater than 3-items
                        size_t k = j - 1;
                        size_t colMinSup = row[j];
                        while (colMinSup >= minSup && row[k] >= minSup)
                        {
                            _temp.insert(0, 1, mapPostionToNode(k - i - 1));
                            FIs.push_back(temp + _temp);
                            colMinSup--;
                            k--;
                        }
                    }
                    j--;
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
    for (const auto& [label, occurrence] : nodes)
    {
        ss << label << " | ";
        for (const auto& [label, occurrence] : nodes)
        {
            ss << label << " ";
        }
        ss << "| ";
    }
    ss << '\n';

    for (const auto& [label, row] : CLM)
    {
        ss << '\t' << label << " | ";

        int j = 0;
        for (const auto& val : row)
        {
            if (j == 1)
            {
                ss << "| ";
            }

            ss << val << ' ';

            j++;
            if (j == (maxNodes + 1))
            {
                ss << "| ";
                j = 0;
            }
        }

        ss << '\n';
    }

    return ss.str();
}
