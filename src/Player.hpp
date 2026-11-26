#pragma once

#include "World.hpp"

#include "raylib.h"

class Player
{
public:
    void Spawn(const World& world);
    void Update(const World& world, float deltaTime, int moveDirection, bool jumpPressed);
    void Draw() const;

    Rectangle GetBounds() const;
    Vector2 GetCenter() const;

private:
    bool Collides(const World& world, Rectangle bounds) const;

    Vector2 position_{0.0F, 0.0F};
    Vector2 velocity_{0.0F, 0.0F};
    bool onGround_ = false;

    static constexpr float Width = 22.0F;
    static constexpr float Height = 54.0F;
    static constexpr float MoveSpeed = 190.0F;
    static constexpr float JumpSpeed = 430.0F;
    static constexpr float Gravity = 1100.0F;
};
