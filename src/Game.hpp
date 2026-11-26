#pragma once

#include "Player.hpp"
#include "World.hpp"

#include "raylib.h"

#include <vector>

class Game
{
public:
    void Run();

private:
    enum class TouchTool
    {
        Mine,
        Place
    };

    struct TouchControls
    {
        Rectangle moveLeft;
        Rectangle moveRight;
        Rectangle jump;
        Rectangle mine;
        Rectangle place;
        Rectangle block;
    };

    void Update(float deltaTime);
    void Draw() const;
    void DrawHud() const;
    void DrawTouchControls() const;
    TouchControls GetTouchControls() const;
    void ProcessTouchInput(int& moveDirection, bool& jumpPressed);
    void HandleBlockAction();
    void HandleTouchBlockAction(Vector2 screenPosition);
    void ApplyBlockAction(int tileX, int tileY, TouchTool tool);
    bool IsInReach(int tileX, int tileY) const;

    World world_;
    Player player_;
    Camera2D camera_{};
    TileType selectedBlock_ = TileType::Dirt;
    TouchTool selectedTool_ = TouchTool::Mine;
    std::vector<int> activeTouchIds_;
};
