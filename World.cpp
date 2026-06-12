#include <algorithm>

#include "World.h"
#include "GameData.h"
#include "EntityPools.h"
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

    // Pre-reserve pool capacity
    m_Zombies.Reserve();
    m_Bullets.Reserve();
    m_Particles.Reserve();
    m_Orbs.Reserve();
}

void World::CreateZombie(float x, float y)
{
    Random::Init();
    bool isMutation = (Random::Float() < 0.05f); // Have 5 percent to be a mutation zombie
    float mutationMultiplier = isMutation ? 2.0f : 1.0f;

    m_Zombies.Add(
        x, y, 0.0f, 50.0f * SpeedMultiplier,
        100.0f * mutationMultiplier * HealthMultiplier,
        100.0f * mutationMultiplier * HealthMultiplier,
        18.0f, 1.0f, 0, isMutation ? (uint8_t)1 : (uint8_t)0
    );
}

void World::CreateBoss(float x, float y)
{
    m_Zombies.Add(
        x, y, 0.0f, 40.0f * SpeedMultiplier,
        100.0f * 10.0f * HealthMultiplier, 100.0f * 10.0f * HealthMultiplier,
        18.0f * 3.0f, 3.0f, 1, 0
    );
}

void World::CreateBullet(float x, float y, float dirX, float dirY)
{
    float bvx = 0.0f, bvy = 0.0f;
    if (dirX != 0.0f || dirY != 0.0f)
    {
        float len = std::sqrt(dirX * dirX + dirY * dirY);
        bvx = (dirX / len) * BulletPool::Speed;
        bvy = (dirY / len) * BulletPool::Speed;
    }

    m_Bullets.Add(x, y, bvx, bvy, 3.0f);
}

void World::CreateParticle(float x, float y, uint32_t color)
{
    Random::Init();
    float angle = Random::Float() * 2.0f * PI;
    float speed = Random::Float() * 100.0f + 20.0f;
    float vx = std::cosf(angle) * speed;
    float vy = std::sinf(angle) * speed;

    float radius = 4.0f + ((vx * vx + vy * vy) > 0.0f ? 1.5f : 0.0f);

    m_Particles.Add(x, y, vx, vy, 0.5f,0.5f, radius, color);
}

void World::CreateOrb(float x, float y, OrbType type)
{
    uint32_t color = 0xFFFFFFFF;
    switch (type)
    {
    case OrbType::Rare:      color = 0x2EFF30FF; break;
    case OrbType::Epic:      color = 0xDD00EFCC; break;
    case OrbType::Legendary: color = 0xFFBD00CC; break;
    default:                 color = 0xFFFFFFFF; break;
    }

    m_Orbs.Add(x, y, 5.0f, color, type);
}

void World::CreateBloodExplosion(float x, float y, int count)
{
    for (int i = 0; i < count; ++i)
        CreateParticle(x, y, 0xFF1A1AFF);
}

//  Main loop

void World::Update(float delta)
{
    // Restart on lose screen
    if (!Play && GameOverMessage && IsKeyPressed(KEY_R))
    {
        Reset();
        return;
    }

    if (Play)
    {
        UpdateCollision (delta);
        UpdatePlayer (delta);
        UpdateZombies (delta);
        UpdateBullets (delta);
        UpdateParticles (delta);

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

        if (waveDuration < 0.0f && CountZombies() > 0)
        {
            Play = false;
            GameOverMessage = "TIME'S UP";
        }
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

        // Clamp camera so the viewport never shows outside the map
        int mapHalfW = renderer.GetTileMapRowSize();
        int mapHalfH = renderer.GetTileMapColSize();
        int screenW  = GetScreenWidth();
        int screenH  = GetScreenHeight();

        int minVx = -mapHalfW;
        int maxVx =  mapHalfW - screenW;
        int minVy = -mapHalfH;
        int maxVy =  mapHalfH - screenH;

        if (vx < minVx) vx = minVx;
        if (vx > maxVx) vx = maxVx;
        if (vy < minVy) vy = minVy;
        if (vy > maxVy) vy = maxVy;

        renderer.SetViewOffset(vx, vy);
    }

    // Tilemap
    renderer.RenderTilemap();

    // Player
    if (m_Player.alive)
        renderer.RenderMob((int)m_Player.x, (int)m_Player.y, m_Player.rotation, 1.0f, EntityType::Player, m_Player.shouldDraw, false);

    // Zombies
    for (size_t i = 0; i < m_Zombies.count; ++i)
    {
        renderer.RenderMob((int)m_Zombies.x[i], (int)m_Zombies.y[i],
                           m_Zombies.rotation[i], m_Zombies.sizeMultiply[i],
                           EntityType::Zombie, true, m_Zombies.isMutation[i]);

        int hpWidth = renderer.GetHealthBarWidth();
        int fill = (int)(hpWidth * (m_Zombies.hp[i] / m_Zombies.maxHp[i]));
        int barOffset = m_Zombies.isBoss[i] ? 50 : 25;
        renderer.RenderHealthBar((int)m_Zombies.x[i] - hpWidth / 2, (int)m_Zombies.y[i] + barOffset, fill);
    }

    // Bullets
    for (size_t i = 0; i < m_Bullets.count; ++i)
    {
        renderer.RenderBullet((int)m_Bullets.x[i], (int)m_Bullets.y[i]);
    }

    // Particles
    for (size_t i = 0; i < m_Particles.count; ++i)
    {
        float t = m_Particles.lifeTime[i] / m_Particles.maxLifeTime[i];
        float radius = m_Particles.radius[i] * t;
        if (radius < 0.5f) continue;

        unsigned char alpha = (unsigned char)(255.0f * t);
        Color inner = GetColor(m_Particles.color[i]);
        inner.a = alpha;
        Color outer = { 140, 10, 10, alpha };

        renderer.RenderParticle((int)m_Particles.x[i], (int)m_Particles.y[i], radius, inner, outer);
    }

    // Orbs (SoA)
    for (size_t i = 0; i < m_Orbs.count; ++i)
    {
        Color inner = GetColor(m_Orbs.color[i]);
        Color outer = inner;
        outer.a = (unsigned char)(255 * 0.2f);

        renderer.RenderSoulOrb((int)m_Orbs.x[i], (int)m_Orbs.y[i], m_Orbs.radius[i], inner, outer);
    }

    if (Play)
    {
        if (m_Player.alive)
        {
            renderer.RenderText(TextFormat("HP: %.0f", m_Player.hp), 5, 0, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Soul: %d", m_Player.collectedSoul), 5, 30, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Wave: %d", currentWave), 5, 60, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Time: %.0f", waveDuration), 450, 10, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Zombie(s) Left: %d", CountZombies()), 5, 530, 30, 0, 0x0000008F);
        }
    }

    // Game Over screen
    if (!Play && GameOverMessage)
    {
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        // Dark overlay
        DrawRectangle(0, 0, screenW, screenH, { 0, 0, 0, 180 });

        // "YOU LOSE" title
        int titleSize = 60;
        int titleWidth = MeasureText("YOU LOSE", titleSize);
        DrawText("YOU LOSE", (screenW - titleWidth) / 2, screenH / 2 - 80, titleSize, RED);

        // Reason ("YOU DIED" or "TIME'S UP")
        int reasonSize = 36;
        int reasonWidth = MeasureText(GameOverMessage, reasonSize);
        DrawText(GameOverMessage, (screenW - reasonWidth) / 2, screenH / 2, reasonSize, { 255, 200, 200, 255 });

        // Stats
        const char* waveText = TextFormat("Wave Reached: %d", currentWave);
        const char* soulText = TextFormat("Souls Collected: %d", m_Player.collectedSoul);
        int statsSize = 24;

        int waveWidth = MeasureText(waveText, statsSize);
        DrawText(waveText, (screenW - waveWidth) / 2, screenH / 2 + 60, statsSize, LIGHTGRAY);

        int soulWidth = MeasureText(soulText, statsSize);
        DrawText(soulText, (screenW - soulWidth) / 2, screenH / 2 + 90, statsSize, LIGHTGRAY);

        // Restart hint
        const char* restartText = "Press R to Restart";
        int restartSize = 20;
        int restartWidth = MeasureText(restartText, restartSize);
        DrawText(restartText, (screenW - restartWidth) / 2, screenH / 2 + 140, restartSize, { 200, 200, 200, 200 });
    }
}

void World::Reset()
{
    // Clear all entity pools
    m_Zombies.Clear();
    m_Bullets.Clear();
    m_Particles.Clear();
    m_Orbs.Clear();

    // Reset player
    CreatePlayer(0.0f, 0.0f);

    // Reset game state
    Play = true;
    GameOverMessage = nullptr;

    // Reset wave state
    currentWave = 0;
    waveDuration = 60.0f;
    waveDelay = 3.0f;
    isWaveClear = true;
    m_lastFifthWave = -1;

    // Reset difficulty
    SpawnRadius = 500.0f;
    MaxZombies = 50;
    HealthMultiplier = 1.0f;
    SpeedMultiplier = 1.0f;

    // Reset screen shake
    ShakeTrauma = 0.0f;
}

void World::UpdateCollision(float delta)
{
    if (!m_Player.alive || m_Player.invulnerable) return;

    for (size_t i = 0; i < m_Zombies.count; ++i)
    {
        float dx = m_Player.x - m_Zombies.x[i];
        float dy = m_Player.y - m_Zombies.y[i];
        float rSum = m_Player.hitboxRadius + m_Zombies.hitboxRadius[i];

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

            if (m_Player.hp <= 0.0f)
            {
                Play = false;
                GameOverMessage = "YOU DIED";
            }

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

    Renderer& renderer = Application::GetRenderer();

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

    // Clamp player inside the map
    if (m_Player.x >= renderer.GetTileMapRowSize())
        m_Player.x = renderer.GetTileMapRowSize();
    if (m_Player.x <= -renderer.GetTileMapRowSize())
        m_Player.x = -renderer.GetTileMapRowSize();

    if (m_Player.y >= renderer.GetTileMapColSize())
        m_Player.y = renderer.GetTileMapColSize();
    if (m_Player.y <= -renderer.GetTileMapColSize())
        m_Player.y = -renderer.GetTileMapColSize();

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
    for (size_t i = 0; i < m_Orbs.count; )
    {
        float odx = m_Player.x - m_Orbs.x[i];
        float ody = m_Player.y - m_Orbs.y[i];
        float distSq = odx * odx + ody * ody;

        // Attract when inside collect range
        if (distSq < m_Player.collectRange * m_Player.collectRange)
        {
            float dist = std::sqrt(distSq);
            if (dist > 0.0f)
            {
                float absorbSpeed = 5.0f * dist;
                m_Orbs.x[i] += (odx / dist) * absorbSpeed * delta;
                m_Orbs.y[i] += (ody / dist) * absorbSpeed * delta;
            }
        }

        // Collect on touch
        float collectDist = m_Player.hitboxRadius + m_Orbs.radius[i];
        float cdx = m_Player.x - m_Orbs.x[i];
        float cdy = m_Player.y - m_Orbs.y[i];
        if (cdx * cdx + cdy * cdy <= collectDist * collectDist)
        {
            int soulValue = 1;
            switch (m_Orbs.type[i])
            {
            case OrbType::Rare:      soulValue = 3;  break;
            case OrbType::Epic:      soulValue = 7;  break;
            case OrbType::Legendary: soulValue = 15; break;
            default:                 soulValue = 1;  break;
            }
            m_Player.collectedSoul += soulValue;
            m_Orbs.Remove(i);
        }
        else
        {
            ++i;
        }
    }

    // Mouse aim
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

    for (size_t i = 0; i < m_Zombies.count; ++i)
    {
        float dx = m_Player.x - m_Zombies.x[i];
        float dy = m_Player.y - m_Zombies.y[i];

        if (dx != 0.0f || dy != 0.0f)
        {
            float len = std::sqrt(dx * dx + dy * dy);
            m_Zombies.x[i] += (dx / len) * m_Zombies.speed[i] * delta;
            m_Zombies.y[i] += (dy / len) * m_Zombies.speed[i] * delta;
        }

        m_Zombies.rotation[i] = atan2f(m_Player.y - m_Zombies.y[i],
                                       m_Player.x - m_Zombies.x[i]) * RAD2DEG;
    }
}

void World::UpdateBullets(float delta)
{
    for (size_t i = 0; i < m_Bullets.count; )
    {
        m_Bullets.x[i] += m_Bullets.vx[i] * delta;
        m_Bullets.y[i] += m_Bullets.vy[i] * delta;

        m_Bullets.lifeTime[i] -= delta;
        if (m_Bullets.lifeTime[i] <= 0.0f)
        {
            m_Bullets.Remove(i);
            continue;
        }

        // Bullet vs zombie
        Vector2 bulletCenter = { m_Bullets.x[i], m_Bullets.y[i] };
        bool bulletHit = false;

        for (size_t z = 0; z < m_Zombies.count; )
        {
            Vector2 zombieCenter = { m_Zombies.x[z], m_Zombies.y[z] };
            if (CheckCollisionCircles(bulletCenter, BulletPool::HitboxRadius,
                                      zombieCenter, m_Zombies.hitboxRadius[z]))
            {
                m_Zombies.hp[z] -= 100.0f;
                ShakeTrauma = 0.6f;

                if (m_Zombies.hp[z] <= 0.0f)
                {
                    CreateBloodExplosion(m_Zombies.x[z], m_Zombies.y[z], 10);

                    // Roll random orb type based on zombie kind
                    Random::Init();
                    float roll = Random::Float();
                    OrbType dropType = OrbType::Common;

                    if (m_Zombies.isBoss[z])
                    {
                        // Boss: 30% Epic, 70% Legendary
                        dropType = (roll < 0.3f) ? OrbType::Epic : OrbType::Legendary;
                    }
                    else if (m_Zombies.isMutation[z])
                    {
                        // Mutation: 10% Common, 60% Rare, 30% Epic
                        if (roll < 0.1f)       dropType = OrbType::Common;
                        else if (roll < 0.7f)  dropType = OrbType::Rare;
                        else                   dropType = OrbType::Epic;
                    }
                    else
                    {
                        // Normal: 80% Common, 20% Rare
                        dropType = (roll < 0.8f) ? OrbType::Common : OrbType::Rare;
                    }

                    CreateOrb(m_Zombies.x[z], m_Zombies.y[z], dropType);
                    m_Zombies.Remove(z);
                }
                else
                {
                    CreateBloodExplosion(m_Zombies.x[z], m_Zombies.y[z], 2);
                    ++z;
                }

                bulletHit = true;
                break;
            }
            else
            {
                ++z;
            }
        }

        if (bulletHit)
        {
            m_Bullets.Remove(i);
        }
        else
        {
            ++i;
        }
    }
}

void World::UpdateParticles(float delta)
{
    for (size_t i = 0; i < m_Particles.count; )
    {
        m_Particles.x[i] += m_Particles.vx[i] * delta;
        m_Particles.y[i] += m_Particles.vy[i] * delta;

        m_Particles.lifeTime[i] -= delta;
        if (m_Particles.lifeTime[i] <= 0.0f)
        {
            m_Particles.Remove(i);
            // Don't increment — re-check swapped-in element
        }
        else
        {
            ++i;
        }
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
    return (int)m_Zombies.count;
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