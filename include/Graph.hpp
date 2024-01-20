#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <map>
#include <sstream>
#include <vector>

class Graph
{
    int maxSize;
    int maxRowSize;
    std::map<char, std::vector<int>> CLM;

public:
    explicit Graph(const int maxSize) : maxSize(maxSize), maxRowSize(maxSize * maxSize + maxSize)
    {
        for (int i = 0; i < maxSize; ++i)
        {
            CLM.emplace(65 + i, maxRowSize);
        }
    }

    void processTransaction(const std::string &str)
    {
        for (int startIndex = 0; startIndex < str.length(); ++startIndex)
        {
            const char startChar = str[startIndex];
            const int startCharPos = (startChar % 65) * (maxSize + 1);
            CLM[startChar][startCharPos]++;

            for (int offsetIndex = startIndex + 1; offsetIndex < str.length(); ++offsetIndex)
            {
                const int endCharOffset = str[offsetIndex] % 65 * (maxSize + 1);
                CLM[startChar][endCharOffset]++;

                for (int i = offsetIndex + 1; i < str.length(); ++i)
                {
                    CLM[startChar][endCharOffset + str[i] % 65 + 1]++;
                }
            }
        }
    }

    [[nodiscard]] std::string toString() const noexcept
    {
        std::stringstream ss;

        ss << "CLM: \n\t  | ";
        for (int i = 0; i < maxSize; ++i)
        {
            ss << static_cast<char>(65 + i) << " | ";
            for (int j = 0; j < maxSize; ++j)
            {
                ss << static_cast<char>(65 + j) << " ";
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
                if (j == (maxSize + 1))
                {
                    ss << "| ";
                    j = 0;
                }
            }

            ss << '\n';
        }

        return ss.str();
    }
};

#endif //GRAPH_HPP