#include "Game.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace
{
#if !defined(PLATFORM_ANDROID)
constexpr int ScreenWidth = 1280;
constexpr int ScreenHeight = 720;
#endif
constexpr float ReachInTiles = 6.0F;

#if defined(PLATFORM_ANDROID)
bool ContainsPoint(Rectangle bounds, Vector2 point)
{
    return CheckCollisionPointRec(point, bounds);
}
#endif
}

void Game::Run()
{
#if defined(PLATFORM_ANDROID)
    InitWindow(0, 0, "Pixel World");
#else
    InitWindow(ScreenWidth, ScreenHeight, "Pixel World");
#endif
    SetTargetFPS(60);

    player_.Spawn(world_);
    camera_.offset = {GetScreenWidth() * 0.5F, GetScreenHeight() * 0.5F};
    camera_.target = player_.GetCenter();
    camera_.rotation = 0.0F;
    camera_.zoom = 1.0F;

    while (!WindowShouldClose())
    {
        const float deltaTime = std::min(GetFrameTime(), 1.0F / 30.0F);
        Update(deltaTime);
        Draw();
    }

    CloseWindow();
}

void Game::Update(float deltaTime)
{
    const bool keyRight = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    const bool keyLeft = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    int moveDirection = (keyRight ? 1 : 0) - (keyLeft ? 1 : 0);
    bool jumpPressed = IsKeyPressed(KEY_SPACE);
    ProcessTouchInput(moveDirection, jumpPressed);

    player_.Update(world_, deltaTime, moveDirection, jumpPressed);
    camera_.target = player_.GetCenter();

    if (IsKeyPressed(KEY_ONE))
    {
        selectedBlock_ = TileType::Dirt;
    }
    else if (IsKeyPressed(KEY_TWO))
    {
        selectedBlock_ = TileType::Stone;
    }

    HandleBlockAction();
}

Game::TouchControls Game::GetTouchControls() const
{
    const float width = static_cast<float>(GetScreenWidth());
    const float height = static_cast<float>(GetScreenHeight());
    const float buttonSize = std::clamp(std::min(width, height) * 0.13F, 52.0F, 88.0F);
    const float gap = buttonSize * 0.14F;
    const float margin = buttonSize * 0.18F;
    const float bottom = height - margin - buttonSize;

    return {
        {margin, bottom, buttonSize, buttonSize},
        {margin + buttonSize + gap, bottom, buttonSize, buttonSize},
        {width - margin - buttonSize, bottom, buttonSize, buttonSize},
        {width - margin - buttonSize * 3.0F - gap * 2.0F, bottom, buttonSize, buttonSize},
        {width - margin - buttonSize * 2.0F - gap, bottom, buttonSize, buttonSize},
        {width - margin - buttonSize, margin, buttonSize, buttonSize}};
}

void Game::ProcessTouchInput(int& moveDirection, bool& jumpPressed)
{
#if defined(PLATFORM_ANDROID)
    const TouchControls controls = GetTouchControls();
    bool moveLeft = false;
    bool moveRight = false;
    std::vector<int> currentTouchIds;
    const int touchCount = GetTouchPointCount();
    currentTouchIds.reserve(static_cast<std::size_t>(touchCount));

    for (int i = 0; i < touchCount; ++i)
    {
        const int touchId = GetTouchPointId(i);
        const Vector2 position = GetTouchPosition(i);
        const bool justPressed =
            std::find(activeTouchIds_.begin(), activeTouchIds_.end(), touchId) == activeTouchIds_.end();
        currentTouchIds.push_back(touchId);

        if (ContainsPoint(controls.moveLeft, position))
        {
            moveLeft = true;
            continue;
        }
        if (ContainsPoint(controls.moveRight, position))
        {
            moveRight = true;
            continue;
        }

        if (ContainsPoint(controls.jump, position))
        {
            if (justPressed)
            {
                jumpPressed = true;
            }
            continue;
        }
        if (ContainsPoint(controls.mine, position))
        {
            if (justPressed)
            {
                selectedTool_ = TouchTool::Mine;
            }
            continue;
        }
        if (ContainsPoint(controls.place, position))
        {
            if (justPressed)
            {
                selectedTool_ = TouchTool::Place;
            }
            continue;
        }
        if (ContainsPoint(controls.block, position))
        {
            if (justPressed)
            {
                selectedBlock_ = selectedBlock_ == TileType::Dirt ? TileType::Stone : TileType::Dirt;
            }
            continue;
        }
        if (justPressed)
        {
            HandleTouchBlockAction(position);
        }
    }

    moveDirection += (moveRight ? 1 : 0) - (moveLeft ? 1 : 0);
    moveDirection = std::clamp(moveDirection, -1, 1);
    activeTouchIds_ = std::move(currentTouchIds);
#else
    (void)moveDirection;
    (void)jumpPressed;
#endif
}

bool Game::IsInReach(int tileX, int tileY) const
{
    const Vector2 center = player_.GetCenter();
    const float tileCenterX = (static_cast<float>(tileX) + 0.5F) * World::TileSize;
    const float tileCenterY = (static_cast<float>(tileY) + 0.5F) * World::TileSize;
    const float dx = center.x - tileCenterX;
    const float dy = center.y - tileCenterY;
    const float reach = ReachInTiles * World::TileSize;
    return dx * dx + dy * dy <= reach * reach;
}

void Game::ApplyBlockAction(int tileX, int tileY, TouchTool tool)
{
    if (!IsInReach(tileX, tileY))
    {
        return;
    }

    if (tool == TouchTool::Mine)
    {
        if (IsSolid(world_.GetTile(tileX, tileY)))
        {
            world_.SetTile(tileX, tileY, TileType::Air);
        }
        return;
    }

    if (world_.GetTile(tileX, tileY) != TileType::Air)
    {
        return;
    }

    const Rectangle tileBounds{
        static_cast<float>(tileX * World::TileSize),
        static_cast<float>(tileY * World::TileSize),
        static_cast<float>(World::TileSize),
        static_cast<float>(World::TileSize)};
    if (!CheckCollisionRecs(tileBounds, player_.GetBounds()))
    {
        world_.SetTile(tileX, tileY, selectedBlock_);
    }
}

void Game::HandleTouchBlockAction(Vector2 screenPosition)
{
    const Vector2 worldPosition = GetScreenToWorld2D(screenPosition, camera_);
    const int tileX = static_cast<int>(std::floor(worldPosition.x / World::TileSize));
    const int tileY = static_cast<int>(std::floor(worldPosition.y / World::TileSize));
    ApplyBlockAction(tileX, tileY, selectedTool_);
}

void Game::HandleBlockAction()
{
#if !defined(PLATFORM_ANDROID)
    const Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera_);
    const int tileX = static_cast<int>(std::floor(mouseWorld.x / World::TileSize));
    const int tileY = static_cast<int>(std::floor(mouseWorld.y / World::TileSize));
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && IsSolid(world_.GetTile(tileX, tileY)))
    {
        ApplyBlockAction(tileX, tileY, TouchTool::Mine);
    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        ApplyBlockAction(tileX, tileY, TouchTool::Place);
    }
#endif
}

void Game::Draw() const
{
    ClearBackground(Color{125, 190, 235, 255});

    BeginMode2D(camera_);
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    const float halfWidth = screenWidth / (2.0F * camera_.zoom);
    const float halfHeight = screenHeight / (2.0F * camera_.zoom);
    const int minX = static_cast<int>(std::floor((camera_.target.x - halfWidth) / World::TileSize)) - 1;
    const int maxX = static_cast<int>(std::floor((camera_.target.x + halfWidth) / World::TileSize)) + 1;
    const int minY = static_cast<int>(std::floor((camera_.target.y - halfHeight) / World::TileSize)) - 1;
    const int maxY = static_cast<int>(std::floor((camera_.target.y + halfHeight) / World::TileSize)) + 1;
    world_.Draw(minX, maxX, minY, maxY);
    player_.Draw();

#if !defined(PLATFORM_ANDROID)
    const Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera_);
    const int mouseTileX = static_cast<int>(std::floor(mouseWorld.x / World::TileSize));
    const int mouseTileY = static_cast<int>(std::floor(mouseWorld.y / World::TileSize));
    if (IsInReach(mouseTileX, mouseTileY))
    {
        DrawRectangleLines(mouseTileX * World::TileSize, mouseTileY * World::TileSize,
                           World::TileSize, World::TileSize, WHITE);
    }
#endif
    EndMode2D();

    DrawHud();
#if defined(PLATFORM_ANDROID)
    DrawTouchControls();
#endif
}

void Game::DrawHud() const
{
    DrawRectangle(14, 14, 386, 91, Fade(Color{20, 25, 32, 255}, 0.88F));
    DrawText("PIXEL WORLD", 26, 22, 20, RAYWHITE);
#if defined(PLATFORM_ANDROID)
    DrawText("Tap a block to mine or place", 26, 50, 16, RAYWHITE);
    DrawText("Choose tool and block at right", 26, 72, 16, RAYWHITE);
#else
    DrawText("Move: A/D or arrows    Jump: Space", 26, 50, 16, RAYWHITE);
    DrawText("Mine: Left click    Place: Right click", 26, 72, 16, RAYWHITE);
#endif

    const Color blockColor = selectedBlock_ == TileType::Dirt
                                 ? Color{139, 91, 57, 255}
                                 : Color{108, 112, 121, 255};
    const int screenHeight = GetScreenHeight();
#if !defined(PLATFORM_ANDROID)
    DrawRectangle(14, screenHeight - 58, 254, 44, Fade(Color{20, 25, 32, 255}, 0.88F));
    DrawRectangle(24, screenHeight - 48, 24, 24, blockColor);
    DrawRectangleLines(24, screenHeight - 48, 24, 24, RAYWHITE);
    DrawText(selectedBlock_ == TileType::Dirt ? "1: Dirt selected" : "2: Stone selected",
             58, screenHeight - 46, 18, RAYWHITE);
#else
    (void)screenHeight;
    (void)blockColor;
#endif
}

void Game::DrawTouchControls() const
{
    const TouchControls controls = GetTouchControls();
    const auto drawButton = [](Rectangle bounds, const char* label, bool active)
    {
        const Color background = active ? Color{70, 116, 155, 225} : Color{25, 31, 40, 190};
        DrawRectangleRounded(bounds, 0.22F, 6, background);
        DrawRectangleRoundedLines(bounds, 0.22F, 6, Fade(RAYWHITE, 0.72F));
        const int fontSize = static_cast<int>(bounds.height * 0.24F);
        const int textWidth = MeasureText(label, fontSize);
        DrawText(label,
                 static_cast<int>(bounds.x + (bounds.width - textWidth) * 0.5F),
                 static_cast<int>(bounds.y + (bounds.height - fontSize) * 0.5F),
                 fontSize, RAYWHITE);
    };

    drawButton(controls.moveLeft, "<", false);
    drawButton(controls.moveRight, ">", false);
    drawButton(controls.jump, "JUMP", false);
    drawButton(controls.mine, "MINE", selectedTool_ == TouchTool::Mine);
    drawButton(controls.place, "PLACE", selectedTool_ == TouchTool::Place);
    drawButton(controls.block, selectedBlock_ == TileType::Dirt ? "DIRT" : "STONE", false);
}
