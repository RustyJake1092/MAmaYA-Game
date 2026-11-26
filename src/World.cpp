#include "World.hpp"

#include "raylib.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

std::uint64_t World::MakeKey(int x, int y)
{
    return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(x)) << 32U) |
           static_cast<std::uint32_t>(y);
}

float World::Hash01(int value, int salt)
{
    std::uint32_t hash = static_cast<std::uint32_t>(value);
    hash ^= static_cast<std::uint32_t>(salt) * 0x9E3779B9U;
    hash ^= hash >> 16U;
    hash *= 0x7FEB352DU;
    hash ^= hash >> 15U;
    hash *= 0x846CA68BU;
    hash ^= hash >> 16U;
    return static_cast<float>(hash) / static_cast<float>(UINT32_MAX);
}

float World::ValueNoise(float x, int period, int salt)
{
    const float gridPosition = x / static_cast<float>(period);
    const int left = static_cast<int>(std::floor(gridPosition));
    const float fraction = gridPosition - static_cast<float>(left);
    const float blend = fraction * fraction * (3.0F - 2.0F * fraction);
    const float first = Hash01(left, salt);
    const float second = Hash01(left + 1, salt);
    return first + (second - first) * blend;
}

int World::GetSurfaceY(int x) const
{
    const float broadNoise = (ValueNoise(static_cast<float>(x), 64, 1) * 2.0F - 1.0F) * 8.0F;
    const float detailNoise = (ValueNoise(static_cast<float>(x), 17, 2) * 2.0F - 1.0F) * 3.0F;
    return 14 + static_cast<int>(std::round(broadNoise + detailNoise));
}

TileType World::GetTile(int x, int y) const
{
    const auto change = changes_.find(MakeKey(x, y));
    if (change != changes_.end())
    {
        return change->second;
    }

    const int surfaceY = GetSurfaceY(x);
    if (y < surfaceY)
    {
        return TileType::Air;
    }
    if (y == surfaceY)
    {
        return TileType::Grass;
    }
    if (y <= surfaceY + 4)
    {
        return TileType::Dirt;
    }
    return TileType::Stone;
}

void World::SetTile(int x, int y, TileType type)
{
    changes_[MakeKey(x, y)] = type;
}

void World::Draw(int minX, int maxX, int minY, int maxY) const
{
    for (int x = minX; x <= maxX; ++x)
    {
        for (int y = minY; y <= maxY; ++y)
        {
            const TileType type = GetTile(x, y);
            if (type == TileType::Air)
            {
                continue;
            }

            const int pixelX = x * TileSize;
            const int pixelY = y * TileSize;
            Color color = type == TileType::Grass ? Color{91, 166, 69, 255}
                         : type == TileType::Dirt ? Color{139, 91, 57, 255}
                                                  : Color{108, 112, 121, 255};
            DrawRectangle(pixelX, pixelY, TileSize, TileSize, color);

            if (type == TileType::Grass)
            {
                DrawRectangle(pixelX, pixelY, TileSize, 6, Color{127, 196, 79, 255});
            }

            DrawRectangleLines(pixelX, pixelY, TileSize, TileSize, Fade(BLACK, 0.12F));
        }
    }
}
