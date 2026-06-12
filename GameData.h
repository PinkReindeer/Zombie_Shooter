#pragma once

static constexpr int WorldRange = 1500;

enum class EntityType : uint8_t
{
    None   = 0,
    Player = 1,
    Zombie = 2,
    Bullet = 3
};

enum class OrbType : uint8_t
{
    None      = 0,
    Common    = 1,
    Rare      = 2,
    Epic      = 3,
    Legendary = 4
};

struct Player
{
    float x = 0.0f, y = 0.0f;
    float rotation = 0.0f;
    float speed = 150.0f;
    float hitboxRadius = 18.0f;

    float hp = 100.0f;
    float maxHp = 100.0f;

    float invulnerableTimer = 0.0f;
    float shootTimer = 0.0f;
    float knockVx = 0.0f, knockVy = 0.0f;
    float collectRange = 100.0f;

    int  collectedSoul = 0;
    bool invulnerable = false;
    bool shouldDraw = true;
    bool alive = false;

    static constexpr float ShootCooldown = 0.3f;
    static constexpr float KnockbackForce = 500.0f;
    static constexpr float InvulnerableTime = 1.0f;
};
