#ifndef GRAPH_H
#define GRAPH_H

#include "json.hpp"
#include <list>
#include <sstream>
#include <string>

class Graph {
  std::list<std::string> transactions;
  std::list<nlohmann::json> jsonHistory;
  std::list<std::pair<char, int>> nodes;
  std::list<std::tuple<char, char, std::list<char>, int>> rawEdges;
  std::vector<std::vector<int>> CLM;

  /**
   * \brief Checks if the edge exists, if so, increments the weight
   * \param fromNode The from node
   * \param toNode The to node
   * \param extraNodes The extra nodes
   * \return True if the weight was incremented otherwise false if edge was not
   * found
   */
  bool incrementIfRawEdgeExists(const char fromNode, const char toNode,
                                const std::list<char> &extraNodes) noexcept;

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

  /**
   * \brief Returns an image of the graphs current state excluding json history
   * \return A JSON object representing the graph
   */
  [[nodiscard]] nlohmann::json toJSONObject() const noexcept;

public:
  /**
   * \brief Processes the passed string into the graph generating new nodes and
   * edges \param str The transaction to process
   */
  void processTransaction(const std::string &str);

  /**
   * \brief Processes the graph data to fill the CLM
   */
  void processCLM();

  /**
   * \return The JSON history in chronological order (0 being the oldest)
   */
  [[nodiscard]] std::string getJSONHistoryAsJSON() const noexcept;

  /**
   * \brief Serializes all of the graph data excluding JSON history into a JSON
   * nlohmann::json \return The JSON string
   */
  [[nodiscard]] std::string toJSON() const noexcept;
  [[nodiscard]] std::string toPrettyJSON(const int indent = 2) const noexcept;

  /**
   * \brief Serializes all of the graph data excluding JSON history into a
   * string \return The resultant string
   */
  [[nodiscard]] std::string toString() const noexcept;
};

#endif // GRAPH_H
