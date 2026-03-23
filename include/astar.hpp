#ifndef WYPAS_CORE_ASTAR_HPP
#define WYPAS_CORE_ASTAR_HPP

#include "position.hpp"
#include "defs.hpp"
#include <bitset>
#include <cstdint>
#include <vector>

namespace core {

struct AStarNode {
    uint16_t x = 0;
    uint16_t y = 0;
    AStarNode* parent = nullptr;
    int32_t f = 0;
    int32_t g = 0;
    int32_t h = 0;
};

using WalkCostFn = int (*)(int x, int y, int z, void* userdata);

class AStarNodes {
public:
    AStarNodes();

    AStarNode* createOpenNode();
    AStarNode* getBestNode();
    AStarNode* getNodeAt(uint16_t x, uint16_t y);

    void openNode(AStarNode* node);
    void closeNode(AStarNode* node);

    bool isInList(uint16_t x, uint16_t y) const;

    static int32_t getEstimatedDistance(uint16_t x, uint16_t y, uint16_t xGoal, uint16_t yGoal);
    static int32_t getMoveCost(const AStarNode* node, uint16_t nx, uint16_t ny);

private:
    AStarNode nodes_[MAX_NODES];
    std::bitset<MAX_NODES> openNodes_;
    uint32_t curNode_ = 0;
};

bool findPath(const Position& start, const Position& target,
              int32_t maxDist, uint8_t floor,
              WalkCostFn walkCost, void* userdata,
              std::vector<Direction>& dirs);

} // namespace core

#endif
