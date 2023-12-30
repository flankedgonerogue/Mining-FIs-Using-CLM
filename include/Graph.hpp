#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <string>
#include "json.hpp"

class Graph
{
    struct Node {
      char label{};
      size_t occurrence{};

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

    size_t maxNodes = 1;
    size_t maxRowSize = maxNodes * maxNodes + maxNodes;
    std::list<Node> nodes;
    std::list<Edge> edges;
    std::map<char, std::vector<size_t>> CLM;

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
     * \brief Maps node to an integer postion to use for building CLM
     * \param node The node to map
     * \return The position of the node
     */
    [[nodiscard]] size_t mapNodeToPosition(char node) const noexcept;
    [[nodiscard]] char mapPostionToNode(size_t node) const noexcept;

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Graph, nodes, edges, CLM);

    explicit Graph(const size_t maxNodes): maxNodes(maxNodes) {}
    Graph() = default;

    void setMaxNodes(size_t maxNodes) noexcept;

    /**
     * \brief Processes the passed string into the graph generating new nodes and
     * edges \param str The transaction to process
     */
    void processTransaction(const std::string &str);

    /**
 * \brief Processes FIs above the minimum support using the CLM Miner algorithm
 * \param minSup The minimum support count
 * \return A list of FIs that have support count above or equal to minSup after using the CLM Miner
 */
    std::list<std::string> useCLM_Miner(int minSup);

    /**
     * \brief Serializes all of the graph data excluding JSON history into a
     * string \return The resultant string
     */
    [[nodiscard]] std::string toString() const noexcept;
};

#endif // GRAPH_H
