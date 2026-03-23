#include <cmath>
#include <format>
#include <cassert>

#include "ECS/World.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentTypes.h"
#include "Systems/PlayerSystem.h"
#include "Systems/ZombieSystem.h"
#include "Systems/BulletSystem.h"
#include "Systems/CollisionSystem.h"
#include "Systems/ParticleSystem.h"
#include "Systems/RenderSystem.h"
#include "Graphics/Renderer.h"
#include "Application.h"
#include "Random.h"

World::World()
{
}

EntityId World::CreatePlayer(float x, float y)
{
    EntityId id = m_EntityManager.CreateEntity();
    m_EntityManager.AddComponents(id, COMP_TRANSFORM | COMP_HEALTH | COMP_MOB_RENDER | COMP_PLAYER_STATE);

    auto& transform = m_EntityManager.Transform(id);
    transform.x = x;
    transform.y = y;

    auto& health = m_EntityManager.Health(id);
    health.hp = 100.0f;
    health.maxHp = 100.0f;

    auto& mob = m_EntityManager.MobRender(id);
    mob.type = EntityType::Player;
    mob.hitboxRadius = 18.0f;
    mob.speed = 150.0f;
    mob.shouldDraw = true;

    m_PlayerID = id;
    return id;
}

#include <iostream>

EntityId World::CreateZombie(float x, float y)
{
    EntityId id = m_EntityManager.CreateEntity();
    m_EntityManager.AddComponents(id, COMP_TRANSFORM | COMP_HEALTH | COMP_MOB_RENDER | COMP_ZOMBIE_TAG);

    bool isMutation = false;
    float multiplier = 1.0f;

    Random::Init();
    if (Random::Float() < 0.05f)
    {
        isMutation = true;
        multiplier = 2.0f;
    }

    auto& transform = m_EntityManager.Transform(id);
    transform.x = x;
    transform.y = y;

    auto& health = m_EntityManager.Health(id);
    health.hp = 100.0f * multiplier * HealthMultiplier;
    health.maxHp = 100.0f * multiplier * HealthMultiplier;

    auto& mob = m_EntityManager.MobRender(id);
    mob.type = EntityType::Zombie;
    mob.hitboxRadius = 18.0f;
    mob.speed = 50.0f;
    mob.isMutation = isMutation;
    mob.shouldDraw = true;

    return id;
}

EntityId World::CreateBoss(float x, float y)
{
    EntityId id = m_EntityManager.CreateEntity();
    m_EntityManager.AddComponents(id, COMP_TRANSFORM | COMP_HEALTH | COMP_MOB_RENDER | COMP_ZOMBIE_TAG | COMP_BOSS_TAG);

    auto& transform = m_EntityManager.Transform(id);
    transform.x = x;
    transform.y = y;

    auto& health = m_EntityManager.Health(id);
    health.hp = 100.0f * 10.0f * HealthMultiplier;
    health.maxHp = 100.0f * 10.0f * HealthMultiplier;

    auto& mob = m_EntityManager.MobRender(id);
    mob.type = EntityType::Zombie;
    mob.hitboxRadius = 18.0f * 3.0f;
    mob.speed = 40.0f;
    mob.sizeMultiply = 3.0f;
    mob.shouldDraw = true;

    return id;
}

EntityId World::CreateBullet(float x, float y, float dirX, float dirY)
{
    EntityId id = m_EntityManager.CreateEntity();
    m_EntityManager.AddComponents(id, COMP_TRANSFORM | COMP_BULLET);

    auto& transform = m_EntityManager.Transform(id);
    transform.x = x;
    transform.y = y;

    auto& bullet = m_EntityManager.Bullet(id);
    bullet.lifeTime = 3.0f;

    if (dirX != 0.0f || dirY != 0.0f)
    {
        float len = std::sqrt(dirX * dirX + dirY * dirY);
        bullet.vx = (dirX / len) * BulletComponent::Speed;
        bullet.vy = (dirY / len) * BulletComponent::Speed;
    }

    return id;
}

EntityId World::CreateOrb(float x, float y, OrbType type)
{
    EntityId id = m_EntityManager.CreateEntity();
    m_EntityManager.AddComponents(id, COMP_TRANSFORM | COMP_ORB);

    auto& transform = m_EntityManager.Transform(id);
    transform.x = x;
    transform.y = y;

    auto& orb = m_EntityManager.Orb(id);
    switch (type)
    {
    case OrbType::Rare:
        orb.color = 0x2EFF30FF;
        break;
    case OrbType::Epic:
        orb.color = 0xDD00EFCC;
        break;
    case OrbType::Legendary:
        orb.color = 0xFFBD00CC;
        break;
    default:
        orb.color = 0xFFFFFFFF;
        break;
    }
    orb.type = type;

    return id;
}

void World::CreateBloodExplosion(float x, float y, int count)
{
    for (int i = 0; i < count; ++i)
    {
        CreateParticle(x, y, 0xFF1A1AFF); // RED
    }
}

// Increase the collected orb then destroy them
void World::CollectOrb(EntityId id)
{
    auto& playerState = m_EntityManager.PlayerState(m_PlayerID);
    playerState.collectedSoul += 1;

    DestroyEntity(id);
}

EntityId World::CreateParticle(float x, float y, uint32_t color)
{
    EntityId id = m_EntityManager.CreateEntity();
    m_EntityManager.AddComponents(id, COMP_TRANSFORM | COMP_PARTICLE);

    auto& transform = m_EntityManager.Transform(id);
    transform.x = x;
    transform.y = y;

    Random::Init();
    float angle = Random::Float() * 2.0f * PI;
    float speed = Random::Float() * 100.0f + 20.0f;
    float vx = std::cosf(angle) * speed;
    float vy = std::sinf(angle) * speed;

    auto& particle = m_EntityManager.Particle(id);
    particle.vx = vx;
    particle.vy = vy;
    particle.lifeTime = 0.5f;
    particle.maxLifeTime = 0.5f;
    particle.radius = 4.0f + (vx * vx + vy * vy > 0 ? 1.5f : 0.0f);
    particle.color = color;

    return id;
}

void World::DestroyEntity(EntityId id)
{
    if (id == m_PlayerID)
        m_PlayerID = NULL_ENTITY;

    m_EntityManager.DestroyEntity(id);
}

void World::Update(float delta)
{
    if (Play)
    {
        CollisionSystem{}.Update(*this, delta);
        PlayerSystem{}.Update(*this, delta);
        ZombieSystem{}.Update(*this, delta);
        BulletSystem{}.Update(*this, delta);
        ParticleSystem{}.Update(*this, delta);

        SpawnTimer -= delta;
        waveDuration -= delta;
        

        // Start a new wave after the wave is cleared
        if (CountZombies() <= 0)
        {
            isWaveClear = true;
            waveDelay -= delta;
            if (waveDelay <= 0.0f)
            {
                ++currentWave;
                StartWaves();
            }
        }

        if (waveDuration < 0) waveDuration = 60.0f;

        UpdateDifficulty(delta);
    }

    // Decay screen shake
    if (ShakeTrauma > 0.0f)
        ShakeTrauma = std::max(0.0f, ShakeTrauma - delta / ShakeDuration);
}

void World::Render()
{
    Renderer& renderer = Application::GetRenderer();

    if (m_PlayerID != NULL_ENTITY)
    {
        const auto& transform = m_EntityManager.Transform(m_PlayerID);
        int vx = (int)transform.x - GetScreenWidth() / 2;
        int vy = (int)transform.y - GetScreenHeight() / 2;

        // Apply screen shake
        if (ShakeTrauma > 0.0f)
        {
            float power = ShakeTrauma * ShakeTrauma;
            vx += (int)(Random::FloatSigned() * ShakeIntensity * power);
            vy += (int)(Random::FloatSigned() * ShakeIntensity * power);
        }

        renderer.SetViewOffset(vx, vy);
    }

    RenderSystem{}.Render(*this, renderer);
}

void World::UpdateDifficulty(float delta)
{
    GameDifficulty += DifficultyGrowth * delta;

    float baseSpawnInterval = 2.0f;
    float spawnDelay = baseSpawnInterval / (1.0f + GameDifficulty);
    if (spawnDelay < 0.3f) spawnDelay = 0.3f;

    SpawnInterval = spawnDelay;
    HealthMultiplier = 1.0f + GameDifficulty * 0.1f;
}

void World::StartWaves()
{
    int totalZombie = std::min(currentWave * 5, 100);

    if (isWaveClear)
    {
        if (currentWave % 25 == 0) 
            for (int i = 0; i < 3; ++i) SpawnEnemy(true);
        else if (currentWave % 5 == 0) 
            SpawnEnemy(true);
        else 
            for (int i = 0; i < totalZombie; ++i) SpawnEnemy();
    }

    waveDelay = 3.0f;
    isWaveClear = false;
}
int World::CountZombies() const
{
    int count = 0;
    for (EntityId id = 0; id < m_EntityManager.Capacity(); ++id)
    {
        if (m_EntityManager.HasComponent(id, COMP_ZOMBIE_TAG))
            ++count;
    }

    return count;
}

void World::SpawnEnemy(bool isBoss)
{
    assert(m_PlayerID != NULL_ENTITY);

    const auto& transform = m_EntityManager.Transform(m_PlayerID);
    float zombieX = 0.0f, zombieY = 0.0f;

    Random::Init();
    do 
    {
        zombieX = (float)Random::IntSymmetric(WorldRange);
        zombieY = (float)Random::IntSymmetric(WorldRange);
    }  while ((zombieX * zombieX + zombieY * zombieY) < SpawnRadius * SpawnRadius);

    //float theta = Random::Float() * 2.0f * PI;
    //float zombieX = transform.x + SpawnRadius * cosf(theta);
    //float zombieY = transform.y + SpawnRadius * sinf(theta);

    if (isBoss)
        CreateBoss(zombieX, zombieY);
    else
        CreateZombie(zombieX, zombieY);
}