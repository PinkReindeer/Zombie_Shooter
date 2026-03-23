#include <cmath>

#include "Systems/CollisionSystem.h"
#include "ECS/World.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentTypes.h"

void CollisionSystem::Update(World& world, float delta)
{
    EntityId playerID = world.GetPlayerId();
    if (playerID == NULL_ENTITY) return;

    EntityManager& entityManager = world.GetEM();

    TransformComponent& playerTransform = entityManager.Transform(playerID);
    HealthComponent& playerHp = entityManager.Health(playerID);
    MobRenderComponent& playerMob = entityManager.MobRender(playerID);
    PlayerComponent& playerState = entityManager.PlayerState(playerID);

    if (playerState.invulnerable) return; // nothing to do while invulnerable

    for (EntityId zombieID = 0; zombieID < entityManager.Capacity(); ++zombieID)
    {
        if (!entityManager.HasComponent(zombieID, COMP_ZOMBIE_TAG)) continue;

        const TransformComponent& zombieTransform = entityManager.Transform(zombieID);
        const MobRenderComponent& zombieMob = entityManager.MobRender(zombieID);

        float dx = playerTransform.x - zombieTransform.x;
        float dy = playerTransform.y - zombieTransform.y;
        float rSum = playerMob.hitboxRadius + zombieMob.hitboxRadius;

        if (dx * dx + dy * dy <= rSum * rSum)
        {
            // Damage
            playerHp.hp -= 10.0f;
            if (playerHp.hp < 0.0f) playerHp.hp = 0.0f;

            // Invulnerability
            playerState.invulnerable = true;
            playerState.invulnerableTimer = PlayerComponent::InvulnerableTime;

            // Knockback direction away from zombie
            float len = std::sqrt(dx * dx + dy * dy);
            if (len > 0.0f)
            {
                playerState.knockVx += (dx / len) * PlayerComponent::KnockbackForce;
                playerState.knockVy += (dy / len) * PlayerComponent::KnockbackForce;
            }

            // Screen shake
            world.ShakeTrauma = 1.0f;

            // Death check
            if (playerHp.hp <= 0.0f)
                world.Play = false;

            break; // one hit per frame is enough
        }
    }
}
