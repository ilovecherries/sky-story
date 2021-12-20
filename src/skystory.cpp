#include <algorithm>
#include <iostream>

#include <raylib.h>

constexpr float JMP_VEL = 5.5;
constexpr float SPD_VEL = 3;
constexpr float FALL_VEL = 0.25;
constexpr float CAM_SPD = SPD_VEL * 2;

constexpr float WIN_WIDTH = 400;
constexpr float WIN_HEIGHT = 240;

constexpr Color BACKDROP_COLOR = {
    .r = 0,
    .g = 150,
    .b = 200,
    .a = 255};

using std::abs;
using std::max;
using std::min;

class BgMap
{
public:
    void render(const Vector2 offset);
};

enum class Direction
{
    Left,
    Right
};

enum class PlayerState
{
    InAir,
    Idle,
};

class Player;

class GameCamera
{
public:
    Vector2 pos;
    void update(const Player player);
};

class Player
{
    Vector2 vel;

public:
    Vector2 pos;
    Direction dir;
    Texture2D sprite;
    PlayerState state;

    Player()
    {
        this->sprite = LoadTexture("media/player.png");
        this->vel = Vector2{0};
        this->pos = Vector2{0};
    }

    void input()
    {
        if (IsKeyDown(KEY_D))
        {
            this->vel.x = min(SPD_VEL, this->vel.x + SPD_VEL);
            this->dir = Direction::Right;
        }
        if (IsKeyDown(KEY_A))
        {
            this->vel.x = max(-SPD_VEL, this->vel.x - SPD_VEL);
            this->dir = Direction::Left;
        }
        if ((this->state != PlayerState::InAir) && IsKeyDown(KEY_SPACE))
        {
            this->vel.y = -JMP_VEL;
            this->state = PlayerState::InAir;
        }
    }

    void update()
    {
        constexpr float TEST_COLL_HEIGHT = 16 * 8;

        this->vel.y += FALL_VEL;

        if (this->vel.x > 0)
            this->vel.x = max(0.0f, this->vel.x - FALL_VEL);
        if (this->vel.x < 0)
            this->vel.x = min(0.0f, this->vel.x + FALL_VEL);

        if (this->pos.y + this->vel.y >= TEST_COLL_HEIGHT)
        {
            this->vel.y = 0;
            this->state = PlayerState::Idle;
        }

        this->pos.x += this->vel.x;
        this->pos.y += this->vel.y;
    }

    void render(const Vector2 offset)
    {
        Vector2 vec = {
            .x = this->pos.x - offset.x,
            .y = this->pos.y + offset.y,
        };
        DrawTextureV(this->sprite, vec, WHITE);
    }
};

template <typename T>
T cameraMove(T from, T to)
{
    constexpr float moveSpeed = 6.0f;
    T c = abs(from - to) / moveSpeed;
    if (to > from)
        return min(to, from + c);
    else if (to < from)
        return max(to, from - c);
    return from;
}

void GameCamera::update(const Player player)
{
    constexpr float HALF_WIDTH = (WIN_WIDTH / 2.0f);
    constexpr float HALF_HEIGHT = (WIN_WIDTH / 2.0f);
    constexpr float PLAYER_OFFSET = (WIN_WIDTH / 6.0f);

    Vector2 dest = {
        .x = player.pos.x - HALF_WIDTH,
        .y = -player.pos.y + HALF_HEIGHT,
    };
    switch (player.dir)
    {
    case Direction::Left:
        dest.x -= PLAYER_OFFSET;
        break;
    case Direction::Right:
        dest.x += PLAYER_OFFSET;
        break;
    }
    this->pos.x = cameraMove(pos.x, dest.x);
    this->pos.y = cameraMove(pos.y, dest.y);
}

int main(void)
{
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Sky Story (PROTOTYPE 0001)");
    SetTargetFPS(60);
    Player player{};
    GameCamera camera{};

    while (!WindowShouldClose())
    {
        player.input();
        player.update();
        camera.update(player);
        BeginDrawing();
        {
            ClearBackground(BACKDROP_COLOR);
            player.render(camera.pos);
        }
        EndDrawing();
    }

    return 0;
}
