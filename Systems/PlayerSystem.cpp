#include <cmath>

#include "Systems/PlayerSystem.h"
#include "ECS/World.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentTypes.h"
#include "Application.h"

void PlayerSystem::Update(World& world, float delta)
{
    EntityId playerID = world.GetPlayerId();
    if (playerID == NULL_ENTITY) return;

    EntityManager& entityManager = world.GetEM();

    TransformComponent& transform = entityManager.Transform(playerID);
    MobRenderComponent& mob = entityManager.MobRender(playerID);
    HealthComponent& hp = entityManager.Health(playerID);
    PlayerComponent& playerState = entityManager.PlayerState(playerID);

    // WASD movement
    float dx = 0.0f, dy = 0.0f;
    if (IsKeyDown(KEY_A)) dx -= 1.0f;
    if (IsKeyDown(KEY_D)) dx += 1.0f;
    if (IsKeyDown(KEY_W)) dy -= 1.0f;
    if (IsKeyDown(KEY_S)) dy += 1.0f;

    if (dx != 0.0f || dy != 0.0f)
    {
        float len = std::sqrt(dx * dx + dy * dy);
        transform.x += (dx / len) * mob.speed * delta;
        transform.y += (dy / len) * mob.speed * delta;
    }

    // Knockback decay
    if (playerState.knockVx != 0.0f || playerState.knockVy != 0.0f)
    {
        transform.x += playerState.knockVx * delta;
        transform.y += playerState.knockVy * delta;

        // Decay to ~5% of initial speed per second
        float decay = powf(0.05f, delta);
        playerState.knockVx *= decay;
        playerState.knockVy *= decay;

        // Clamp tiny residuals to zero
        if (std::abs(playerState.knockVx) < 0.1f) playerState.knockVx = 0.0f;
        if (std::abs(playerState.knockVy) < 0.1f) playerState.knockVy = 0.0f;
    }

    // Invulnerability blink
    if (playerState.invulnerable)
    {
        int interval = (int)(playerState.invulnerableTimer * 15);
        mob.shouldDraw = (interval % 2 == 0);

        playerState.invulnerableTimer -= delta;
        if (playerState.invulnerableTimer <= 0.0f)
        {
            playerState.invulnerable = false;
            playerState.invulnerableTimer = 0.0f;
            mob.shouldDraw  = true;
        }
    }

    // Collect orb
    Vector2 playerCenter = { transform.x, transform.y };

    for (EntityId orbId = 0; orbId < entityManager.Capacity(); ++orbId)
    {
        if (!entityManager.HasComponent(orbId, COMP_ORB)) continue;

        TransformComponent& orbTransform = entityManager.Transform(orbId);
        OrbComponent& orb = entityManager.Orb(orbId);

        Vector2 orbCenter = { orbTransform.x, orbTransform.y };

        // Will be attracted when in collect range
        if (CheckCollisionCircles(orbCenter, orb.radius, playerCenter, playerState.collectRange))
        {
            float dx = transform.x - orbTransform.x;
            float dy = transform.y - orbTransform.y;

            if (dx != 0.0f || dy != 0.0f)
            {
                float len = std::sqrt(dx * dx + dy * dy);
                float absorbSpeed = 5.0f * len;
                orbTransform.x += (dx / len) * absorbSpeed * delta;
                orbTransform.y += (dy / len) * absorbSpeed * delta;
            }
        }
        
        // Will be collected when torch player hitbox
        if (CheckCollisionCircles(orbCenter, orb.radius, playerCenter, mob.hitboxRadius))
        {
            world.CollectOrb(orbId);
        }
    }

    // Mouse aim
    Renderer& renderer = Application::GetRenderer();
    Vector2 mouseScreen = GetMousePosition();
    Vector2 mouseWorld = renderer.ScreenToWorld(mouseScreen);

    float mouseDirX = mouseWorld.x - transform.x;
    float mouseDirY = mouseWorld.y - transform.y;
    float angle     = atan2f(mouseDirY, mouseDirX);
    mob.rotation    = angle * RAD2DEG;

    // Shooting
    playerState.shootTimer -= delta;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && playerState.shootTimer <= 0.0f)
    {
        Vector2 muzzleOffset = { 32.0f, 10.0f };
        float spawnX = transform.x + (muzzleOffset.x * cosf(angle) - muzzleOffset.y * sinf(angle));
        float spawnY = transform.y + (muzzleOffset.x * sinf(angle) + muzzleOffset.y * cosf(angle));

        float bDirX = mouseWorld.x - spawnX;
        float bDirY = mouseWorld.y - spawnY;

        float centerDist = std::sqrt(mouseDirX * mouseDirX + mouseDirY * mouseDirY);
        if (centerDist < mob.hitboxRadius * 2.0f)
        {
            bDirX = cosf(angle);
            bDirY = sinf(angle);
        }

        world.CreateBullet(spawnX, spawnY, bDirX, bDirY);
        playerState.shootTimer = PlayerComponent::ShootCooldown;
    }
}