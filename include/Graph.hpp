#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <string>
#include "json.hpp"
#include <set>

class Graph
{
    struct Node {
      char label{};
      uint occurrence{};

      bool operator<(const Node & rhs) const {
        return label < rhs.label;
      }

      NLOHMANN_DEFINE_TYPE_INTRUSIVE(Node, label, occurrence);
    };

    struct Edge {
      char from{};
      char to{};
      std::list<char> extraNodes;
      uint occurrence{};

      NLOHMANN_DEFINE_TYPE_INTRUSIVE(Edge, from, to, extraNodes, occurrence);
    };

    std::list<std::string> transactions;
    std::list<nlohmann::json> jsonHistory;
    std::list<Node> nodes;
    std::list<Edge> rawEdges;
    std::vector<std::vector<int>> CLM;

    /**
     * \brief Checks if the edge exists, if so, increments the weight
     * \param fromNode The from node
     * \param toNode The to node
     * \param extraNodes The extra nodes
     * \return True if the weight was incremented otherwise false if edge was not
     * found
     */
    bool incrementIfRawEdgeExists(char fromNode, char toNode, const std::list<char> &extraNodes) noexcept;

    /**
     * \return The number of nodes in the graph
     */
    [[nodiscard]] int getNodesCount() const noexcept;

    /**
     * \brief Maps node to an integer postion to use for building CLM
     * \param node The node to map
     * \return The position of the node
     */
    [[nodiscard]] int mapNodeToPosition(char node) const noexcept;

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Graph, transactions, nodes, rawEdges, CLM, jsonHistory);

    /**
     * \brief Processes the passed string into the graph generating new nodes and
     * edges \param str The transaction to process
     */
    void processTransaction(const std::string &str);

    /**
     * \brief Processes the graph data to fill the CLM
     */
    void processCLM();
    std::set<std::string> clmMiner(int);

    /**
     * \brief Serializes all of the graph data excluding JSON history into a
     * string \return The resultant string
     */
    [[nodiscard]] std::string toString() const noexcept;
};

#endif // GRAPH_H
