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

struct Zombie
{
    float x = 0.0f, y = 0.0f;
    float rotation = 0.0f;
    float speed = 50.0f;
    float hp = 100.0f, maxHp = 100.0f;
    float hitboxRadius = 18.0f;
    float sizeMultiply = 1.0f;

    bool isBoss = false;
    bool isMutation = false;
    bool alive = true;
};

struct Bullet
{
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
    float lifeTime = 3.0f;
    bool alive = true;

    static constexpr float Speed = 600.0f;
    static constexpr float HitboxRadius = 4.0f;
};

struct Particle
{
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
    float lifeTime = 0.5f;
    float maxLifeTime = 0.5f;
    float radius = 5.0f;
    uint32_t color = 0xFFFFFFFF;
    bool alive = true;
};

struct Orb
{
    float x = 0.0f, y = 0.0f;
    float radius = 5.0f;
    uint32_t color = 0xFFFFFFFF;
    OrbType type = OrbType::None;
    bool alive = true;
};
