#include <vector>

#include "Systems/ParticleSystem.h"
#include "ECS/World.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentTypes.h"

void ParticleSystem::Update(World& world, float delta)
{
    EntityManager& entityManager = world.GetEM();

    std::vector<EntityId> toDestroy;

    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, COMP_PARTICLE)) continue;

        TransformComponent& transform = entityManager.Transform(id);
        ParticleComponent& particle = entityManager.Particle(id);

        // Move
        transform.x += particle.vx * delta;
        transform.y += particle.vy * delta;

        // Tick lifetime
        particle.lifeTime -= delta;
        if (particle.lifeTime <= 0.0f)
            toDestroy.push_back(id);
    }

    for (EntityId id : toDestroy)
        world.DestroyEntity(id);
}