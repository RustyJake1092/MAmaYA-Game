#include "Player.hpp"

#include <algorithm>
#include <cmath>

void Player::Spawn(const World& world)
{
    const int spawnColumn = 0;
    position_.x = static_cast<float>(spawnColumn * World::TileSize) +
                  (static_cast<float>(World::TileSize) - Width) * 0.5F;
    position_.y = static_cast<float>(world.GetSurfaceY(spawnColumn) * World::TileSize) - Height;
    velocity_ = {0.0F, 0.0F};
    onGround_ = false;
}

Rectangle Player::GetBounds() const
{
    return {position_.x, position_.y, Width, Height};
}

Vector2 Player::GetCenter() const
{
    return {position_.x + Width * 0.5F, position_.y + Height * 0.5F};
}

bool Player::Collides(const World& world, Rectangle bounds) const
{
    const int minX = static_cast<int>(std::floor(bounds.x / World::TileSize));
    const int maxX = static_cast<int>(std::floor((bounds.x + bounds.width - 0.01F) / World::TileSize));
    const int minY = static_cast<int>(std::floor(bounds.y / World::TileSize));
    const int maxY = static_cast<int>(std::floor((bounds.y + bounds.height - 0.01F) / World::TileSize));

    for (int tileX = minX; tileX <= maxX; ++tileX)
    {
        for (int tileY = minY; tileY <= maxY; ++tileY)
        {
            if (IsSolid(world.GetTile(tileX, tileY)))
            {
                return true;
            }
        }
    }
    return false;
}

void Player::Update(const World& world, float deltaTime, int moveDirection, bool jumpPressed)
{
    velocity_.x = static_cast<float>(moveDirection) * MoveSpeed;

    if (onGround_ && jumpPressed)
    {
        velocity_.y = -JumpSpeed;
        onGround_ = false;
    }

    velocity_.y = std::min(velocity_.y + Gravity * deltaTime, 700.0F);

    position_.x += velocity_.x * deltaTime;
    if (Collides(world, GetBounds()))
    {
        if (velocity_.x > 0.0F)
        {
            const int tileX = static_cast<int>(std::floor((position_.x + Width) / World::TileSize));
            position_.x = static_cast<float>(tileX * World::TileSize) - Width;
        }
        else if (velocity_.x < 0.0F)
        {
            const int tileX = static_cast<int>(std::floor(position_.x / World::TileSize));
            position_.x = static_cast<float>((tileX + 1) * World::TileSize);
        }
    }

    position_.y += velocity_.y * deltaTime;
    onGround_ = false;
    if (Collides(world, GetBounds()))
    {
        if (velocity_.y > 0.0F)
        {
            const int tileY = static_cast<int>(std::floor((position_.y + Height) / World::TileSize));
            position_.y = static_cast<float>(tileY * World::TileSize) - Height;
            onGround_ = true;
        }
        else if (velocity_.y < 0.0F)
        {
            const int tileY = static_cast<int>(std::floor(position_.y / World::TileSize));
            position_.y = static_cast<float>((tileY + 1) * World::TileSize);
        }
        velocity_.y = 0.0F;
    }
}

void Player::Draw() const
{
    const int x = static_cast<int>(std::round(position_.x));
    const int y = static_cast<int>(std::round(position_.y));

    DrawRectangle(x + 2, y + 18, static_cast<int>(Width) - 4, 30, Color{65, 116, 190, 255});
    DrawRectangle(x + 3, y, static_cast<int>(Width) - 6, 20, Color{232, 183, 137, 255});
    DrawRectangle(x + 3, y + 4, static_cast<int>(Width) - 6, 5, Color{92, 59, 43, 255});
    DrawRectangle(x + 5, y + 48, 6, 6, Color{54, 48, 59, 255});
    DrawRectangle(x + 13, y + 48, 6, 6, Color{54, 48, 59, 255});
    DrawRectangle(x + 15, y + 9, 3, 3, BLACK);
}
