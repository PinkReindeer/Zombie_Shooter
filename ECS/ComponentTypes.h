#pragma once

#include <cstdint>

using EntityId = uint32_t;
static constexpr EntityId NULL_ENTITY = UINT32_MAX;

enum class EntityType : uint8_t
{
    None = 0,
    Player = 1,
    Zombie = 2,
    Bullet = 3
};

enum class OrbType : uint8_t
{
    None = 0,
    Common = 1,
    Rare = 2,
    Epic = 3,
    Legendary = 4
};

enum ComponentFlag : uint32_t
{
    COMP_NONE         = 0,
    COMP_TRANSFORM    = 1 << 0,
    COMP_HEALTH       = 1 << 1,
    COMP_MOB_RENDER   = 1 << 2,
    COMP_PLAYER_STATE = 1 << 3,
    COMP_ZOMBIE_TAG   = 1 << 4,
    COMP_BOSS_TAG     = 1 << 5,
    COMP_BULLET       = 1 << 6,
    COMP_PARTICLE     = 1 << 7,
    COMP_ORB          = 1 << 8
};

struct TransformComponent
{
    float x = 0.0f;
    float y = 0.0f;
};

struct HealthComponent
{
    float hp = 100.0f;
    float maxHp = 100.0f;
    int fillAmount = 0;
};

struct MobRenderComponent
{
    float rotation = 0.0f;
    float hitboxRadius = 18.0f;
    float speed = 150.0f;
    float sizeMultiply = 1.0f;
    EntityType type = EntityType::None;
    bool shouldDraw = true;
    bool isMutation = false;
};

struct PlayerComponent
{
    float invulnerableTimer = 0.0f;
    float shootTimer = 0.0f;
    float knockVx = 0.0f;
    float knockVy = 0.0f;
    float collectRange = 100.0f;

    int collectedSoul = 0;
    bool invulnerable = false;

    static constexpr float ShootCooldown = 0.3f;
    static constexpr float KnockbackForce = 500.0f;
    static constexpr float InvulnerableTime = 1.0f;
};

struct ZombieTagComponent {};

struct BossTagComponent {};

struct BulletComponent
{
    float vx = 0.0f;
    float vy = 0.0f;
    float lifeTime = 3.0f;

    static constexpr float Speed = 600.0f;
    static constexpr float HitboxRadius = 4.0f;
};

struct ParticleComponent
{
    float vx = 0.0f;
    float vy = 0.0f;
    float lifeTime = 0.5f;
    float maxLifeTime = 0.5f;
    float radius = 5.0f;
    uint32_t color = 0xFFFFFFFF;
};

struct OrbComponent
{
    float radius = 5.0f;
    float angle = 0.0f;
    uint32_t color = 0xFFFFFFFF;
    OrbType type = OrbType::None;
};