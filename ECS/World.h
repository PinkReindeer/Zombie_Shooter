#pragma once

#include <vector>
#include <string>

#include "ECS/EntityManager.h"

static constexpr int WorldRange = 1500;

class Renderer;

class World
{
public:
    World();

    void Update(float delta);
    void Render();

    EntityId CreatePlayer(float x, float y);
    EntityId CreateZombie(float x, float y);
    EntityId CreateBoss(float x, float y);
    EntityId CreateParticle(float x, float y, uint32_t color);
    EntityId CreateOrb(float x, float y, OrbType type);
    EntityId CreateBullet(float x, float y, float dirX, float dirY);

    void CreateBloodExplosion(float x, float y, int count);

    void CollectOrb(EntityId id);
    void DestroyEntity(EntityId id);

    EntityManager& GetEM() { return m_EntityManager; }
    const EntityManager& GetEM() const { return m_EntityManager; }
    EntityId GetPlayerId() const { return m_PlayerID; }

    int CountZombies() const;

    bool Play = true;

    float GameDifficulty = 0.0f;
    float DifficultyGrowth = 0.05f;

    float SpawnInterval = 2.0f;
    float SpawnTimer = 0.0f;
    float SpawnRadius = 500.0f;
    int MaxZombies = 50;
    float HealthMultiplier = 1.0f;

    // Screen shake
    float ShakeTrauma    = 0.0f;  // 0..1
    float ShakeDuration  = 0.3f;
    float ShakeIntensity = 14.0f;

    // Wave
    int currentWave = 0;
    float waveDuration = 60.0f;
    float waveDelay = 3.0f;
    bool isWaveClear = true;

private:
    void UpdateDifficulty(float delta);
    void StartWaves();
    void SpawnEnemy(bool isBoss = false);

    EntityManager m_EntityManager;
    EntityId m_PlayerID = NULL_ENTITY;
};