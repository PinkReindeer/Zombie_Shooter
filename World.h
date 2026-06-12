#pragma once

#include "GameData.h"
#include "EntityPools.h"
#include "Graphics/Tilemap.h"

class World
{
public:
    void Update(float delta);
    void Render();
    void Reset();

    // Entity factory methods
    void CreatePlayer(float x, float y);
    void CreateZombie(float x, float y);
    void CreateBoss(float x, float y);
    void CreateBullet(float x, float y, float dirX, float dirY);
    void CreateParticle(float x, float y, uint32_t color);
    void CreateOrb(float x, float y, OrbType type);
    void CreateBloodExplosion(float x, float y, int count);

    int CountZombies() const;

    bool Play = true;
    const char* GameOverMessage = nullptr;

    // Difficulty
    float SpawnRadius = 500.0f;
    int MaxZombies = 50;
    float HealthMultiplier = 1.0f;
    float SpeedMultiplier = 1.0f;

    // Screen shake
    float ShakeTrauma = 0.0f;
    float ShakeDuration = 0.3f;
    float ShakeIntensity = 14.0f;

    // Wave
    int currentWave = 0;
    float waveDuration = 60.0f;
    float waveDelay = 3.0f;
    bool  isWaveClear = true;

private:
    // Per-frame update passes
    void UpdateCollision(float delta);
    void UpdatePlayer(float delta);
    void UpdateZombies(float delta);
    void UpdateBullets(float delta);
    void UpdateParticles(float delta);

    // Wave / spawn helpers
    void StartWaves();
    void SpawnBoss();
    void SpawnZombie();
    void UpdateDifficulty();
    Vector2 GenerateRandomSpawnPosition() const;

    // Data pools (SoA)
    Player m_Player;
    Tilemap m_Tilemap;

    ZombiePool   m_Zombies;
    BulletPool   m_Bullets;
    ParticlePool m_Particles;
    OrbPool      m_Orbs;

    int m_lastFifthWave = -1;
};
