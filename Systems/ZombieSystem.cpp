#include <cmath>

#include "Systems/ZombieSystem.h"
#include "ECS/World.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentTypes.h"

void ZombieSystem::Update(World& world, float delta)
{
    EntityId playerID = world.GetPlayerId();
    if (playerID == NULL_ENTITY) return;

    EntityManager& entityManager = world.GetEM();

    const TransformComponent& playerTransform = entityManager.Transform(playerID);

    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, COMP_ZOMBIE_TAG)) continue;

        TransformComponent& transform = entityManager.Transform(id);
        MobRenderComponent& mob = entityManager.MobRender(id);

        float dx = playerTransform.x - transform.x;
        float dy = playerTransform.y - transform.y;

        if (dx != 0.0f || dy != 0.0f)
        {
            float len = std::sqrt(dx * dx + dy * dy);
            transform.x += (dx / len) * mob.speed * delta;
            transform.y += (dy / len) * mob.speed * delta;
        }

        mob.rotation = atan2f(playerTransform.y - transform.y, playerTransform.x - transform.x) * RAD2DEG;
    }

    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, COMP_BOSS_TAG)) continue;

        TransformComponent& transform = entityManager.Transform(id);
        MobRenderComponent& mob = entityManager.MobRender(id);

        float dx = playerTransform.x - transform.x;
        float dy = playerTransform.y - transform.y;

        if (dx != 0.0f || dy != 0.0f)
        {
            float len = std::sqrt(dx * dx + dy * dy);
            transform.x += (dx / len) * mob.speed * delta;
            transform.y += (dy / len) * mob.speed * delta;
        }

        mob.rotation = atan2f(playerTransform.y - transform.y, playerTransform.x - transform.x) * RAD2DEG;
    }
}