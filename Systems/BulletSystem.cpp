#include <cmath>
#include <vector>

#include "BulletSystem.h"
#include "Random.h"
#include "ECS/World.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentTypes.h"

void BulletSystem::Update(World& world, float delta)
{
    EntityManager& entityManager = world.GetEM();

    std::vector<EntityId> toDestroy;

    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, COMP_BULLET)) continue;

        TransformComponent& transform = entityManager.Transform(id);
        BulletComponent& bullet = entityManager.Bullet(id);

        // Move
        transform.x += bullet.vx * delta;
        transform.y += bullet.vy * delta;

        // Lifetime
        bullet.lifeTime -= delta;
        if (bullet.lifeTime <= 0.0f)
        {
            toDestroy.push_back(id);
            continue;
        }

        // Bullet vs zombie collision
        bool hit = false;
        Vector2 bulletCenter = { transform.x, transform.y };

        for (EntityId zombieID = 0; zombieID < entityManager.Capacity(); ++zombieID)
        {
            if (!entityManager.HasComponent(zombieID, COMP_ZOMBIE_TAG)) continue;

            const TransformComponent& zombieTransform = entityManager.Transform(zombieID);
            const MobRenderComponent& zombieMob = entityManager.MobRender(zombieID);
            HealthComponent& zombieHp = entityManager.Health(zombieID);

            Vector2 zombieCenter = { zombieTransform.x, zombieTransform.y };
            if (CheckCollisionCircles(bulletCenter, bullet.HitboxRadius, zombieCenter, zombieMob.hitboxRadius))
            {
                zombieHp.hp -= 100.0f;

                // Screen shake
                world.ShakeTrauma = 0.6f;

                // Blood particle 
                if (zombieHp.hp <= 0.0f)
                {
                    world.CreateBloodExplosion(zombieTransform.x, zombieTransform.y, 10);
                    world.CreateOrb(zombieTransform.x, zombieTransform.y, OrbType::Common);
                    world.DestroyEntity(zombieID);
                }
                else
                    world.CreateBloodExplosion(zombieTransform.x, zombieTransform.y, 2);

                hit = true;
                break;
            }
        }

        if (hit)
            toDestroy.push_back(id);
    }

    for (EntityId id : toDestroy)
        world.DestroyEntity(id);
}