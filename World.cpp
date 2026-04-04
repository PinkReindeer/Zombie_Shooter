#include <cmath>
#include <cassert>
#include <algorithm>

#include "World.h"
#include "GameData.h"
#include "Graphics/Renderer.h"
#include "Application.h"
#include "Random.h"

void World::CreatePlayer(float x, float y)
{
    m_Player = {};
    m_Player.x = x;
    m_Player.y = y;
    m_Player.hp = 100.0f;
    m_Player.maxHp = 100.0f;
    m_Player.speed = 150.0f;
    m_Player.hitboxRadius = 18.0f;
    m_Player.shouldDraw = true;
    m_Player.alive = true;
}

void World::CreateZombie(float x, float y)
{
    Random::Init();
    bool isMutation = (Random::Float() < 0.05f); // Have 5 percent to be a mutation zombie
    float mutationMultiplier = isMutation ? 2.0f : 1.0f;

    Zombie zombie;
    zombie.x = x;
    zombie.y = y;
    zombie.hp = 100.0f * mutationMultiplier * HealthMultiplier;
    zombie.maxHp = zombie.hp;
    zombie.speed = 50.0f * SpeedMultiplier;
    zombie.hitboxRadius = 18.0f;
    zombie.sizeMultiply = 1.0f;
    zombie.isMutation = isMutation;
    zombie.isBoss = false;
    zombie.alive = true;

    m_Zombies.emplace_back(zombie);
}

void World::CreateBoss(float x, float y)
{
    Zombie zombie;
    zombie.x = x;
    zombie.y = y;
    zombie.hp = 100.0f * 10.0f * HealthMultiplier;
    zombie.maxHp = zombie.hp;
    zombie.speed = 40.0f * SpeedMultiplier;
    zombie.hitboxRadius = 18.0f * 3.0f;
    zombie.sizeMultiply = 3.0f;
    zombie.isBoss = true;
    zombie.isMutation = false;
    zombie.alive = true;

    m_Zombies.emplace_back(zombie);
}

void World::CreateBullet(float x, float y, float dirX, float dirY)
{
    Bullet bullet;
    bullet.x = x;
    bullet.y = y;
    bullet.lifeTime = 3.0f;
    bullet.alive = true;

    if (dirX != 0.0f || dirY != 0.0f)
    {
        float len = std::sqrt(dirX * dirX + dirY * dirY);
        bullet.vx = (dirX / len) * Bullet::Speed;
        bullet.vy = (dirY / len) * Bullet::Speed;
    }

    m_Bullets.emplace_back(bullet);
}

void World::CreateParticle(float x, float y, uint32_t color)
{
    Random::Init();
    float angle = Random::Float() * 2.0f * PI;
    float speed = Random::Float() * 100.0f + 20.0f;
    float vx = std::cosf(angle) * speed;
    float vy = std::sinf(angle) * speed;

    Particle particle;
    particle.x = x;
    particle.y = y;
    particle.vx = vx;
    particle.vy = vy;
    particle.lifeTime = 0.5f;
    particle.maxLifeTime = 0.5f;
    particle.radius = 4.0f + ((vx * vx + vy * vy) > 0.0f ? 1.5f : 0.0f);
    particle.color = color;
    particle.alive = true;

    m_Particles.emplace_back(particle);
}

void World::CreateOrb(float x, float y, OrbType type)
{
    Orb orb;
    orb.x = x;
    orb.y = y;
    orb.type = type;
    orb.alive = true;
    orb.radius = 5.0f;

    switch (type)
    {
    case OrbType::Rare:      orb.color = 0x2EFF30FF; break;
    case OrbType::Epic:      orb.color = 0xDD00EFCC; break;
    case OrbType::Legendary: orb.color = 0xFFBD00CC; break;
    default:                 orb.color = 0xFFFFFFFF; break;
    }

    m_Orbs.emplace_back(orb);
}

void World::CreateBloodExplosion(float x, float y, int count)
{
    for (int i = 0; i < count; ++i)
        CreateParticle(x, y, 0xFF1A1AFF);
}

//  Main loop

void World::Update(float delta)
{
    if (Play)
    {
        UpdateCollision (delta);
        UpdatePlayer (delta);
        UpdateZombies (delta);
        UpdateBullets (delta);
        UpdateParticles (delta);

        std::erase_if(m_Zombies, [](const Zombie& zombie) { return !zombie.alive; });
        std::erase_if(m_Bullets, [](const Bullet& bullet) { return !bullet.alive; });
        std::erase_if(m_Particles, [](const Particle& particle) { return !particle.alive; });
        std::erase_if(m_Orbs, [](const Orb& orb) { return !orb.alive; });

        waveDuration -= delta;

        // Start a new wave once all zombies are cleared
        if (CountZombies() <= 0)
        {
            isWaveClear = true;
            waveDelay  -= delta;

            if (currentWave > 0 && currentWave % 5 == 0 && currentWave != m_lastFifthWave)
            {
                UpdateDifficulty();
                m_lastFifthWave = currentWave;
            }

            if (waveDelay <= 0.0f)
            {
                ++currentWave;
                waveDuration = 60.0f;
                StartWaves();
            }
        }

        if (waveDuration < 0.0f) waveDuration = 60.0f;
    }

    // Decay screen shake
    if (ShakeTrauma > 0.0f)
        ShakeTrauma = std::max(0.0f, ShakeTrauma - delta / ShakeDuration);
}

void World::Render()
{
    Renderer& renderer = Application::GetRenderer();

    if (m_Player.alive)
    {
        int vx = (int)m_Player.x - GetScreenWidth()  / 2;
        int vy = (int)m_Player.y - GetScreenHeight() / 2;

        if (ShakeTrauma > 0.0f)
        {
            float power = ShakeTrauma * ShakeTrauma;
            vx += (int)(Random::FloatSigned() * ShakeIntensity * power);
            vy += (int)(Random::FloatSigned() * ShakeIntensity * power);
        }

        renderer.SetViewOffset(vx, vy);
    }
    
    // Player
    if (m_Player.alive)
        renderer.RenderMob((int)m_Player.x, (int)m_Player.y, m_Player.rotation, 1.0f, EntityType::Player, m_Player.shouldDraw, false);

    // Zombies
    for (const Zombie& zombie : m_Zombies)
    {
        if (!zombie.alive) continue;

        renderer.RenderMob((int)zombie.x, (int)zombie.y, zombie.rotation, zombie.sizeMultiply, EntityType::Zombie, true, zombie.isMutation);

        int hpWidth = renderer.GetHealthBarWidth();
        int fill = (int)(hpWidth * (zombie.hp / zombie.maxHp));
        int barOffset = zombie.isBoss ? 50 : 25;
        renderer.RenderHealthBar((int)zombie.x - hpWidth / 2, (int)zombie.y + barOffset, fill);
    }

    // Bullets
    for (const Bullet& bullet : m_Bullets)
    {
        if (bullet.alive)
            renderer.RenderBullet((int)bullet.x, (int)bullet.y);
    }

    // Particles
    for (const Particle& particle : m_Particles)
    {
        if (!particle.alive) continue;

        float t = particle.lifeTime / particle.maxLifeTime;
        float radius = particle.radius * t;
        if (radius < 0.5f) continue;

        unsigned char alpha = (unsigned char)(255.0f * t);
        Color inner = GetColor(particle.color);
        inner.a = alpha;
        Color outer = { 140, 10, 10, alpha };

        renderer.RenderParticle((int)particle.x, (int)particle.y, radius, inner, outer);
    }

    // Orbs
    for (const Orb& orb : m_Orbs)
    {
        if (!orb.alive) continue;

        Color inner = GetColor(orb.color);
        Color outer = inner;
        outer.a = (unsigned char)(255 * 0.2f);

        renderer.RenderSoulOrb((int)orb.x, (int)orb.y, orb.radius, inner, outer);
    }

    if (Play)
    {
        if (m_Player.alive)
        {
            renderer.RenderText(TextFormat("HP: %.0f", m_Player.hp), 5, 35, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Soul: %d", m_Player.collectedSoul), 5, 60, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Wave: %d", currentWave), 5, 85, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Time: %.0f", waveDuration), 500, 35, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Zombie(s) Left: %d", CountZombies()), 5, 530, 30, 0, 0x0000008F);
        }
    }
}

void World::UpdateCollision(float delta)
{
    if (!m_Player.alive || m_Player.invulnerable) return;

    for (Zombie& zombie : m_Zombies)
    {
        if (!zombie.alive) continue;

        float dx = m_Player.x - zombie.x;
        float dy = m_Player.y - zombie.y;
        float rSum = m_Player.hitboxRadius + zombie.hitboxRadius;

        if (dx * dx + dy * dy <= rSum * rSum)
        {
            m_Player.hp -= 10.0f;
            if (m_Player.hp < 0.0f) m_Player.hp = 0.0f;

            m_Player.invulnerable = true;
            m_Player.invulnerableTimer = Player::InvulnerableTime;

            float len = std::sqrt(dx * dx + dy * dy);
            if (len > 0.0f)
            {
                m_Player.knockVx += (dx / len) * Player::KnockbackForce;
                m_Player.knockVy += (dy / len) * Player::KnockbackForce;
            }

            ShakeTrauma = 1.0f;

            if (m_Player.hp <= 0.0f) Play = false;

            break;
        }
    }
}

void World::UpdatePlayer(float delta)
{
    if (!m_Player.alive) return;

    // WASD movement
    float dx = 0.0f, dy = 0.0f;
    if (IsKeyDown(KEY_A)) dx -= 1.0f;
    if (IsKeyDown(KEY_D)) dx += 1.0f;
    if (IsKeyDown(KEY_W)) dy -= 1.0f;
    if (IsKeyDown(KEY_S)) dy += 1.0f;

    if (dx != 0.0f || dy != 0.0f)
    {
        float len = std::sqrt(dx * dx + dy * dy);
        m_Player.x += (dx / len) * m_Player.speed * delta;
        m_Player.y += (dy / len) * m_Player.speed * delta;
    }

    // Knockback decay
    if (m_Player.knockVx != 0.0f || m_Player.knockVy != 0.0f)
    {
        m_Player.x += m_Player.knockVx * delta;
        m_Player.y += m_Player.knockVy * delta;

        float decay = powf(0.05f, delta);
        m_Player.knockVx *= decay;
        m_Player.knockVy *= decay;

        if (std::abs(m_Player.knockVx) < 0.1f) m_Player.knockVx = 0.0f;
        if (std::abs(m_Player.knockVy) < 0.1f) m_Player.knockVy = 0.0f;
    }

    // Invulnerability blink
    if (m_Player.invulnerable)
    {
        int interval = (int)(m_Player.invulnerableTimer * 15.0f);
        m_Player.shouldDraw = (interval % 2 == 0);

        m_Player.invulnerableTimer -= delta;
        if (m_Player.invulnerableTimer <= 0.0f)
        {
            m_Player.invulnerable      = false;
            m_Player.invulnerableTimer = 0.0f;
            m_Player.shouldDraw        = true;
        }
    }

    // Orb attraction & collection
    for (Orb& orb : m_Orbs)
    {
        if (!orb.alive) continue;

        float odx = m_Player.x - orb.x;
        float ody = m_Player.y - orb.y;
        float distSq = odx * odx + ody * ody;

        // Attract when inside collect range
        if (distSq < m_Player.collectRange * m_Player.collectRange)
        {
            float dist = std::sqrt(distSq);
            if (dist > 0.0f)
            {
                float absorbSpeed = 5.0f * dist;
                orb.x += (odx / dist) * absorbSpeed * delta;
                orb.y += (ody / dist) * absorbSpeed * delta;
            }
        }

        // Collect on touch
        float collectDist = m_Player.hitboxRadius + orb.radius;
        float cdx = m_Player.x - orb.x;
        float cdy = m_Player.y - orb.y;
        if (cdx * cdx + cdy * cdy <= collectDist * collectDist)
        {
            m_Player.collectedSoul += 1;
            orb.alive = false;
        }
    }

    // Mouse aim
    Renderer& renderer = Application::GetRenderer();
    Vector2 mouseScreen = GetMousePosition();
    Vector2 mouseWorld  = renderer.ScreenToWorld(mouseScreen);

    float mouseDirX = mouseWorld.x - m_Player.x;
    float mouseDirY = mouseWorld.y - m_Player.y;
    float angle     = atan2f(mouseDirY, mouseDirX);
    m_Player.rotation = angle * RAD2DEG;

    // Shooting
    m_Player.shootTimer -= delta;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && m_Player.shootTimer <= 0.0f)
    {
        Vector2 muzzleOffset = { 32.0f, 10.0f };
        float spawnX = m_Player.x + (muzzleOffset.x * cosf(angle) - muzzleOffset.y * sinf(angle));
        float spawnY = m_Player.y + (muzzleOffset.x * sinf(angle) + muzzleOffset.y * cosf(angle));

        float bDirX = mouseWorld.x - spawnX;
        float bDirY = mouseWorld.y - spawnY;

        float centerDist = std::sqrt(mouseDirX * mouseDirX + mouseDirY * mouseDirY);
        if (centerDist < m_Player.hitboxRadius * 2.0f)
        {
            bDirX = cosf(angle);
            bDirY = sinf(angle);
        }

        CreateBullet(spawnX, spawnY, bDirX, bDirY);
        m_Player.shootTimer = Player::ShootCooldown;
    }
}

void World::UpdateZombies(float delta)
{
    if (!m_Player.alive) return;

    for (Zombie& zombie : m_Zombies)
    {
        if (!zombie.alive) continue;

        float dx = m_Player.x - zombie.x;
        float dy = m_Player.y - zombie.y;

        if (dx != 0.0f || dy != 0.0f)
        {
            float len = std::sqrt(dx * dx + dy * dy);
            zombie.x += (dx / len) * zombie.speed * delta;
            zombie.y += (dy / len) * zombie.speed * delta;
        }

        zombie.rotation = atan2f(m_Player.y - zombie.y, m_Player.x - zombie.x) * RAD2DEG;
    }
}

void World::UpdateBullets(float delta)
{
    for (Bullet& bullet : m_Bullets)
    {
        if (!bullet.alive) continue;

        bullet.x += bullet.vx * delta;
        bullet.y += bullet.vy * delta;

        bullet.lifeTime -= delta;
        if (bullet.lifeTime <= 0.0f) { bullet.alive = false; continue; }

        // Bullet vs zombie
        Vector2 bulletCenter = { bullet.x, bullet.y };
        for (Zombie& zombie : m_Zombies)
        {
            if (!zombie.alive) continue;

            Vector2 zombieCenter = { zombie.x, zombie.y };
            if (CheckCollisionCircles(bulletCenter, Bullet::HitboxRadius, zombieCenter, zombie.hitboxRadius))
            {
                zombie.hp -= 100.0f;
                ShakeTrauma = 0.6f;

                if (zombie.hp <= 0.0f)
                {
                    CreateBloodExplosion(zombie.x, zombie.y, 10);
                    CreateOrb(zombie.x, zombie.y, OrbType::Common);
                    zombie.alive = false;
                }
                else
                    CreateBloodExplosion(zombie.x, zombie.y, 2);

                bullet.alive = false;
                break;
            }
        }
    }
}

void World::UpdateParticles(float delta)
{
    for (Particle& particle : m_Particles)
    {
        if (!particle.alive) continue;

        particle.x += particle.vx * delta;
        particle.y += particle.vy * delta;

        particle.lifeTime -= delta;
        if (particle.lifeTime <= 0.0f) particle.alive = false;
    }
}

//  Difficulty & wave spawning

void World::UpdateDifficulty()
{
    HealthMultiplier += 0.25f;
    SpeedMultiplier += 0.1f;
}

void World::StartWaves()
{
    if (!isWaveClear) return;

    int totalZombie = std::min(currentWave * 5, 100);

    if (currentWave % 25 == 0)
        for (int i = 0; i < 3; ++i) SpawnBoss();
    else if (currentWave % 5 == 0)
        SpawnBoss();
    else
        for (int i = 0; i < totalZombie; ++i) SpawnZombie();

    waveDelay = 3.0f;
    isWaveClear = false;
}

void World::SpawnZombie()
{
    assert(m_Player.alive);
    Vector2 position = GenerateRandomSpawnPosition();
    CreateZombie(position.x, position.y);
}

void World::SpawnBoss()
{
    assert(m_Player.alive);
    Vector2 position = GenerateRandomSpawnPosition();
    CreateBoss(position.x, position.y);
}

int World::CountZombies() const
{
    return (int)m_Zombies.size();
}

Vector2 World::GenerateRandomSpawnPosition() const
{
    Vector2 pos = {};
    Random::Init();
    do
    {
        pos.x = (float)Random::IntSymmetric(WorldRange);
        pos.y = (float)Random::IntSymmetric(WorldRange);
    } while ((pos.x * pos.x + pos.y * pos.y) < SpawnRadius * SpawnRadius);
    return pos;
}