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
    const int size_y = getNodesCount();
    const int size_x = size_y * size_y + size_y;
    CLM.resize(size_y);
    for (auto &row : CLM)
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
    jsonHistory.emplace_back(*this);
}

void Graph::processCLM()
{
    std::map<std::string, int> itemsets;

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

    for (const auto &[item, frequency] : itemsets)
    {
        if (item.length() == 1)
        {
            const int pos_y = mapNodeToPosition(item[0]);
            const int pos_x = pos_y + pos_y * getNodesCount();
            CLM[pos_y][pos_x] = frequency;
        }
        else if (item.length() == 2)
        {
            const int pos_y = mapNodeToPosition(item[0]);
            const int pos_x = mapNodeToPosition(item[1]) + mapNodeToPosition(item[1]) * getNodesCount();

            CLM[pos_y][pos_x] = frequency;
        }
        else
        {
            const char fromNode = item[0];
            const char toNode = item[1];

            const char lastNode = item.back();

            const int pos_y = mapNodeToPosition(fromNode);
            const int pos_x = mapNodeToPosition(toNode) + mapNodeToPosition(toNode) * getNodesCount() +
                mapNodeToPosition(lastNode) + 1;

            CLM[pos_y][pos_x] = frequency;
        }
    }
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
        for (const int &key : CLM[i])
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


std::set<std::string> Graph::clmMiner(int minSup) {
    std::set<std::string> mfis;  // Maximal Frequent Itemsets

    // Iterate over all transactions
    for (const std::string& transaction : this->transactions) {
        // Iterate over all items in the transaction
        for (char item : transaction) {
            std::set<std::string> itemSet;
            int itemIndex = this->mapNodeToPosition(item);

            // Iterate over all other items to form pairs
            for (const auto& otherNode : this->nodes) {
                int otherItemIndex = this->mapNodeToPosition(otherNode.label);

                // Make item pair with all other items
                if (itemIndex != otherItemIndex) {
                    itemSet.insert(std::string(1, item) + otherNode.label);
                }
            }

            // Increment value in the CLM accordingly
            for (const auto& pair : itemSet) {
                int pairIndex =
                    this->mapNodeToPosition(pair[1]) + this->mapNodeToPosition(pair[0]) * this->getNodesCount();
                this->CLM[itemIndex][pairIndex]++;
            }
        }
    }

    // Extract Maximal Frequent Itemsets (MFIs)
    for (auto it = this->nodes.begin(); it != this->nodes.end(); ++it) {
        int itemIndex = this->mapNodeToPosition(it->label);

        for (const auto& rowItem : this->nodes) {
            int pairIndex = this->mapNodeToPosition(rowItem.label) +
                            this->mapNodeToPosition(it->label) * this->getNodesCount();
            if (this->CLM[itemIndex][pairIndex] > minSup) {
                std::set<std::string> itemSet;
                itemSet.insert(std::string(1, rowItem.label));
                itemSet.insert(std::string(1, it->label));

                auto start = it;
                std::advance(start, 1);
                for (; start != this->nodes.end(); ++start) {
                    int index = this->mapNodeToPosition(start->label);
                    if (this->CLM[itemIndex][index] > minSup) {
                        itemSet.insert(std::string(1, start->label));
                    }
                }

                // Add MFIs to the set
                for (const auto& item : itemSet) {
                    mfis.insert(item);
                }
            }
        }
    }



    // Print or use the MFIs as needed
    // std::cout << "Maximal Frequent Itemsets (MFIs): ";
    // for (const auto& mfi : mfis) {
    //     std::cout << mfi << " ";
    // }
    // std::cout << std::endl;

    return mfis;
}
