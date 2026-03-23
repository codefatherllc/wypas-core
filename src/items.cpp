#include "items.hpp"
#include <fstream>
#include <cstring>
#include <iostream>

namespace core {

// OTB binary stream reader
namespace {

struct OtbReader {
    const uint8_t* data;
    size_t size;
    size_t pos;

    OtbReader(const uint8_t* d, size_t s) : data(d), size(s), pos(0) {}

    bool canRead(size_t n) const { return pos + n <= size; }

    bool readU8(uint8_t& v) {
        if (!canRead(1)) return false;
        v = data[pos++];
        return true;
    }

    bool readU16(uint16_t& v) {
        if (!canRead(2)) return false;
        std::memcpy(&v, data + pos, 2);
        pos += 2;
        return true;
    }

    bool readU32(uint32_t& v) {
        if (!canRead(4)) return false;
        std::memcpy(&v, data + pos, 4);
        pos += 4;
        return true;
    }

    bool skip(size_t n) {
        if (!canRead(n)) return false;
        pos += n;
        return true;
    }

    bool readString(std::string& s, uint16_t len) {
        if (!canRead(len)) return false;
        s.assign(reinterpret_cast<const char*>(data + pos), len);
        pos += len;
        return true;
    }
};

// OTB escape byte
constexpr uint8_t OTB_ESCAPE = 0xFD;
constexpr uint8_t OTB_NODE_START = 0xFE;
constexpr uint8_t OTB_NODE_END = 0xFF;

// Unescape an OTB node's property data
std::vector<uint8_t> unescapeProps(const uint8_t* data, size_t size) {
    std::vector<uint8_t> out;
    out.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        if (data[i] == OTB_ESCAPE && i + 1 < size) {
            out.push_back(data[++i]);
        } else {
            out.push_back(data[i]);
        }
    }
    return out;
}

struct OtbNode {
    uint8_t type = 0;
    std::vector<uint8_t> props;
    std::vector<OtbNode> children;
};

bool parseOtbNode(const uint8_t* data, size_t size, size_t& pos, OtbNode& node) {
    if (pos >= size || data[pos] != OTB_NODE_START) return false;
    pos++; // skip 0xFE

    if (pos >= size) return false;
    node.type = data[pos++];

    size_t propStart = pos;
    while (pos < size) {
        uint8_t b = data[pos];
        if (b == OTB_NODE_START) {
            node.props = unescapeProps(data + propStart, pos - propStart);
            OtbNode child;
            if (!parseOtbNode(data, size, pos, child)) return false;
            node.children.push_back(std::move(child));
            propStart = pos;
        } else if (b == OTB_NODE_END) {
            if (propStart < pos) {
                auto moreProps = unescapeProps(data + propStart, pos - propStart);
                node.props.insert(node.props.end(), moreProps.begin(), moreProps.end());
            }
            pos++; // skip 0xFF
            return true;
        } else if (b == OTB_ESCAPE) {
            pos += 2;
        } else {
            pos++;
        }
    }
    return false;
}

} // anonymous namespace

Items& Items::getInstance() {
    static Items instance;
    return instance;
}

void Items::clear() {
    items_.clear();
}

void Items::ensureSize(uint16_t id) {
    if (id >= items_.size()) {
        items_.resize(id + 1);
    }
}

const ItemType& Items::getItemType(uint16_t id) const {
    if (id < items_.size()) return items_[id];
    return nullType_;
}

ItemType& Items::getItemType(uint16_t id) {
    ensureSize(id);
    return items_[id];
}

bool Items::addItemType(uint16_t id, const ItemType& type) {
    ensureSize(id);
    items_[id] = type;
    items_[id].id = id;
    return true;
}

bool Items::loadFromOtb(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return false;

    auto fileSize = file.tellg();
    file.seekg(0);

    // Skip 4-byte identifier
    uint32_t identifier;
    file.read(reinterpret_cast<char*>(&identifier), 4);

    std::vector<uint8_t> buffer(static_cast<size_t>(fileSize) - 4);
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    file.close();

    OtbNode root;
    size_t pos = 0;
    if (!parseOtbNode(buffer.data(), buffer.size(), pos, root)) {
        std::cerr << "[Error - Items::loadFromOtb] Failed to parse OTB tree" << std::endl;
        return false;
    }

    for (const auto& itemNode : root.children) {
        if (itemNode.props.empty()) continue;

        ItemType iType;
        iType.group = static_cast<ItemGroup>(itemNode.type);

        OtbReader reader(itemNode.props.data(), itemNode.props.size());

        uint32_t flags;
        if (!reader.readU32(flags)) continue;

        iType.blockSolid = (flags & FLAG_BLOCK_SOLID) != 0;
        iType.blockProjectile = (flags & FLAG_BLOCK_PROJECTILE) != 0;
        iType.blockPathFind = (flags & FLAG_BLOCK_PATHFIND) != 0;
        iType.hasHeight = (flags & FLAG_HAS_HEIGHT) != 0;
        iType.pickupable = (flags & FLAG_PICKUPABLE) != 0;
        iType.movable = (flags & FLAG_MOVABLE) != 0;
        iType.stackable = (flags & FLAG_STACKABLE) != 0;
        iType.alwaysOnTop = (flags & FLAG_ALWAYSONTOP) != 0;
        iType.isVertical = (flags & FLAG_VERTICAL) != 0;
        iType.isHorizontal = (flags & FLAG_HORIZONTAL) != 0;
        iType.isHangable = (flags & FLAG_HANGABLE) != 0;
        iType.lookThrough = (flags & FLAG_LOOKTHROUGH) != 0;
        iType.isAnimation = (flags & FLAG_ANIMATION) != 0;
        iType.walkStack = (flags & FLAG_WALKSTACK) != 0;

        iType.floorChange[CHANGE_DOWN] = (flags & FLAG_FLOORCHANGEDOWN) != 0;
        iType.floorChange[CHANGE_NORTH] = (flags & FLAG_FLOORCHANGENORTH) != 0;
        iType.floorChange[CHANGE_EAST] = (flags & FLAG_FLOORCHANGEEAST) != 0;
        iType.floorChange[CHANGE_SOUTH] = (flags & FLAG_FLOORCHANGESOUTH) != 0;
        iType.floorChange[CHANGE_WEST] = (flags & FLAG_FLOORCHANGEWEST) != 0;

        // Read attributes
        uint8_t attr;
        while (reader.readU8(attr)) {
            uint16_t length;
            if (!reader.readU16(length)) break;

            switch (attr) {
                case ITEM_ATTR_SERVERID: {
                    if (length != 2) { reader.skip(length); break; }
                    uint16_t serverId;
                    if (!reader.readU16(serverId)) break;
                    if (serverId > 20000 && serverId < 20100) {
                        serverId -= 20000;
                    }
                    iType.id = serverId;
                    break;
                }
                case ITEM_ATTR_CLIENTID: {
                    if (length != 2) { reader.skip(length); break; }
                    uint16_t clientId;
                    if (!reader.readU16(clientId)) break;
                    iType.clientId = clientId;
                    break;
                }
                case ITEM_ATTR_SPEED: {
                    if (length != 2) { reader.skip(length); break; }
                    uint16_t speed;
                    if (!reader.readU16(speed)) break;
                    iType.speed = speed;
                    break;
                }
                case ITEM_ATTR_TOPORDER: {
                    if (length != 1) { reader.skip(length); break; }
                    uint8_t topOrder;
                    if (!reader.readU8(topOrder)) break;
                    iType.alwaysOnTopOrder = topOrder;
                    break;
                }
                case ITEM_ATTR_NAME: {
                    std::string name;
                    if (!reader.readString(name, length)) break;
                    iType.name = std::move(name);
                    break;
                }
                default:
                    reader.skip(length);
                    break;
            }
        }

        if (iType.id != 0) {
            addItemType(iType.id, iType);
        }
    }

    return true;
}

} // namespace core
