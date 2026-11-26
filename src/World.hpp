#pragma once

#include "Tile.hpp"

#include <cstdint>
#include <unordered_map>

class World
{
public:
    static constexpr int TileSize = 32;

    TileType GetTile(int x, int y) const;
    void SetTile(int x, int y, TileType type);
    int GetSurfaceY(int x) const;
    void Draw(int minX, int maxX, int minY, int maxY) const;

private:
    static std::uint64_t MakeKey(int x, int y);
    static float ValueNoise(float x, int period, int salt);
    static float Hash01(int value, int salt);

    std::unordered_map<std::uint64_t, TileType> changes_;
};
