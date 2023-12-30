#include "../include/Graph.hpp"

// PRIVATE FUNCTIONS

inline void generateVariations(std::list<std::string> &list, const std::string &str, std::string &current,
                               const int index)
{
    list.push_back(current);

    // Recursively generate variations
    for (int i = index; i < str.length(); ++i)
    {
        current.push_back(str[i]);
        generateVariations(list, str, current, i + 1);
        current.pop_back();
    }
}

bool Graph::incrementIfRawEdgeExists(const char fromNode, const char toNode, const std::list<char> &extraNodes) noexcept
{
    for (auto &[existingFromNode, existingToNode, existingExtraNodes, existingWeight] : rawEdges)
    {
        if (existingFromNode == fromNode && existingToNode == toNode && existingExtraNodes == extraNodes)
        {
            ++existingWeight;
            return true;
        }
    }
    return false;
}

int Graph::getNodesCount() const noexcept { return static_cast<int>(nodes.size()); }

int Graph::mapNodeToPosition(const char node) const noexcept
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

char Graph::mapPostionToNode(const int node) const noexcept
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

void Graph::processTransaction(const std::string &str)
{
    // Save the transaction
    transactions.push_back(str);

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
    N = getNodesCount();
    row_length = N * N + N;

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
    jsonHistory.emplace_back(*this);
}

void Graph::processCLM()
{
    // Generate all variations for itemsets
    std::list<std::string> variations;
    std::string _;
    for (const auto &str : transactions)
    {
        generateVariations(variations, str, _, 0);
    }

    for (const auto &variant : variations)
    {
        if (variant.empty())
            continue;
        itemsets[variant]++;
    }

    // Set up CLM
    for (const auto &node : nodes)
    {
        CLM.emplace(node.label, std::vector<int>(row_length, 0));
    }

    // Fill up the CLM
    for (const auto &[item, frequency] : itemsets)
    {
        if (item.length() == 1)
        {
            const int pos_x = mapNodeToPosition(item[0]) + mapNodeToPosition(item[0]) * getNodesCount();
            CLM[item[0]][pos_x] = frequency;
        }
        else if (item.length() == 2)
        {
            const int pos_x = mapNodeToPosition(item[1]) + mapNodeToPosition(item[1]) * getNodesCount();

            CLM[item[0]][pos_x] = frequency;
        }
        else
        {
            const int pos_x = mapNodeToPosition(item[1]) + mapNodeToPosition(item[1]) * getNodesCount() +
                mapNodeToPosition(item.back()) + 1;

            // Update frequency only if it's greater, do not reduce it.
            if (CLM[item[0]][pos_x] < frequency)
            {
                CLM[item[0]][pos_x] = frequency;
            }
        }
    }
}

std::list<std::string> Graph::processMFIs(const int minSup)
{
    std::list<std::string> MFIs;

    // Iterate over each row
    for (const auto &[label, row] : CLM)
    {
        // Iterate over major columns
        for (int i = 0; i < row_length; i += (N + 1))
        {
            // If the major column support count is greater than the minimum support count, check the minor columns
            if (row[i] >= minSup)
            {
                // Add the row and Major to MFI
                std::string temp;
                temp += label;
                const char _ = mapPostionToNode(i % N);
                if (_ == label)
                {
                    MFIs.push_back(temp);
                    continue;
                }
                temp += _;

                MFIs.push_back(temp);

                std::vector<int> positions(N);
                // Store positions of all minor columns greater than min support
                for (int j = i + 1, k = 0; k < N; ++j, ++k)
                {
                    if (row[j] > minSup)
                    {
                        positions[k] = 1;
                    }
                }

                for (int j = 0; j < positions.size(); j++)
                {
                    if (positions[j] == 0)
                        continue;

                    std::string _temp;
                    _temp += mapPostionToNode(j);
                    MFIs.push_back(temp + _temp);
                    int k = j + 1;
                    while (positions[k] == 1 && k < N)
                    {
                        _temp += mapPostionToNode(k);
                        MFIs.push_back(temp + _temp);
                        ++k;
                    }
                }
            }
        }
    }

    return MFIs;
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
    for (const auto &[fromNode, toNode, extraNodes, weight] : rawEdges)
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
    for (int i = 0; i < getNodesCount(); ++i, ++it)
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

    for (int i = 0; i < getNodesCount(); ++i, ++it)
    {
        ss << "\t" << it->label << " | ";

        int j = 0;
        for (const int &key : CLM.find(it->label)->second)
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
