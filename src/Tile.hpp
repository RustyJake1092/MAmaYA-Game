#pragma once

enum class TileType
{
    Air,
    Grass,
    Dirt,
    Stone
};

inline bool IsSolid(TileType type)
{
    return type != TileType::Air;
}
