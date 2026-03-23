#include "astar.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace core {

AStarNodes::AStarNodes() {
    curNode_ = 0;
    openNodes_.reset();
}

AStarNode* AStarNodes::createOpenNode() {
    if (curNode_ >= MAX_NODES) return nullptr;
    uint32_t idx = curNode_++;
    openNodes_[idx] = 1;
    return &nodes_[idx];
}

AStarNode* AStarNodes::getBestNode() {
    if (!curNode_) return nullptr;

    int32_t bestF = 100000;
    uint32_t bestIdx = 0;
    bool found = false;

    for (uint32_t i = 0; i < curNode_; ++i) {
        if (openNodes_[i] && nodes_[i].f < bestF) {
            found = true;
            bestF = nodes_[i].f;
            bestIdx = i;
        }
    }

    return found ? &nodes_[bestIdx] : nullptr;
}

AStarNode* AStarNodes::getNodeAt(uint16_t x, uint16_t y) {
    for (uint32_t i = 0; i < curNode_; ++i) {
        if (nodes_[i].x == x && nodes_[i].y == y) {
            return &nodes_[i];
        }
    }
    return nullptr;
}

void AStarNodes::openNode(AStarNode* node) {
    auto idx = static_cast<uint32_t>(node - nodes_);
    if (idx < MAX_NODES) openNodes_[idx] = 1;
}

void AStarNodes::closeNode(AStarNode* node) {
    auto idx = static_cast<uint32_t>(node - nodes_);
    if (idx < MAX_NODES) openNodes_[idx] = 0;
}

bool AStarNodes::isInList(uint16_t x, uint16_t y) const {
    for (uint32_t i = 0; i < curNode_; ++i) {
        if (nodes_[i].x == x && nodes_[i].y == y) return true;
    }
    return false;
}

int32_t AStarNodes::getEstimatedDistance(uint16_t x, uint16_t y, uint16_t xGoal, uint16_t yGoal) {
    int32_t dx = std::abs(static_cast<int32_t>(x) - xGoal);
    int32_t dy = std::abs(static_cast<int32_t>(y) - yGoal);
    int32_t diagonal = std::min(dx, dy);
    return (MAP_DIAGONALWALKCOST * diagonal) + (MAP_NORMALWALKCOST * ((dx + dy) - (2 * diagonal)));
}

int32_t AStarNodes::getMoveCost(const AStarNode* node, uint16_t nx, uint16_t ny) {
    if (std::abs(static_cast<int32_t>(node->x) - nx) == std::abs(static_cast<int32_t>(node->y) - ny)) {
        return MAP_DIAGONALWALKCOST;
    }
    return MAP_NORMALWALKCOST;
}

bool findPath(const Position& start, const Position& target,
              int32_t maxDist, uint8_t floor,
              WalkCostFn walkCost, void* userdata,
              std::vector<Direction>& dirs) {
    dirs.clear();

    if (start.z != target.z) return false;
    if (start == target) return true;

    if (maxDist > 0 && start.distanceTo(target) > maxDist) return false;

    AStarNodes nodes;

    AStarNode* startNode = nodes.createOpenNode();
    if (!startNode) return false;

    startNode->x = start.x;
    startNode->y = start.y;
    startNode->parent = nullptr;
    startNode->g = 0;
    startNode->h = AStarNodes::getEstimatedDistance(start.x, start.y, target.x, target.y);
    startNode->f = startNode->h;

    static const int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    static const int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

    while (AStarNode* current = nodes.getBestNode()) {
        if (current->x == target.x && current->y == target.y) {
            // Reconstruct path
            std::vector<Direction> reversed;
            AStarNode* p = current;
            while (p->parent) {
                int ddx = static_cast<int>(p->x) - static_cast<int>(p->parent->x);
                int ddy = static_cast<int>(p->y) - static_cast<int>(p->parent->y);
                reversed.push_back(Position::directionFromDelta(ddx, ddy));
                p = p->parent;
            }
            dirs.assign(reversed.rbegin(), reversed.rend());
            return true;
        }

        nodes.closeNode(current);

        for (int i = 0; i < 8; ++i) {
            int nx = static_cast<int>(current->x) + dx[i];
            int ny = static_cast<int>(current->y) + dy[i];

            if (nx < 0 || ny < 0 || nx > 0xFFFF || ny > 0xFFFF) continue;

            bool isDiagonal = (dx[i] != 0 && dy[i] != 0);
            if (isDiagonal) {
                int cx1 = static_cast<int>(current->x) + dx[i];
                int cy1 = static_cast<int>(current->y);
                int cx2 = static_cast<int>(current->x);
                int cy2 = static_cast<int>(current->y) + dy[i];
                if (walkCost(cx1, cy1, floor, userdata) < 0 ||
                    walkCost(cx2, cy2, floor, userdata) < 0) {
                    continue;
                }
            }

            auto ux = static_cast<uint16_t>(nx);
            auto uy = static_cast<uint16_t>(ny);

            if (nodes.isInList(ux, uy)) {
                AStarNode* existing = nodes.getNodeAt(ux, uy);
                if (existing) {
                    int32_t newG = current->g + AStarNodes::getMoveCost(current, ux, uy);
                    if (newG < existing->g) {
                        existing->parent = current;
                        existing->g = newG;
                        existing->f = newG + existing->h;
                        nodes.openNode(existing);
                    }
                }
                continue;
            }

            int cost = walkCost(nx, ny, floor, userdata);
            if (cost < 0) continue;

            AStarNode* neighbor = nodes.createOpenNode();
            if (!neighbor) return false;

            neighbor->x = ux;
            neighbor->y = uy;
            neighbor->parent = current;
            neighbor->g = current->g + AStarNodes::getMoveCost(current, ux, uy) + cost;
            neighbor->h = AStarNodes::getEstimatedDistance(ux, uy, target.x, target.y);
            neighbor->f = neighbor->g + neighbor->h;
        }
    }

    return false;
}

} // namespace core
