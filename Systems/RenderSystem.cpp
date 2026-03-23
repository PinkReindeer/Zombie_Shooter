#include "RenderSystem.h"
#include "Application.h"
#include "ECS/World.h"
#include "ECS/EntityManager.h"
#include "ECS/ComponentTypes.h"
#include "Graphics/Renderer.h"

void RenderSystem::Render(const World& world, Renderer& renderer)
{
    const EntityManager& entityManager = world.GetEM();

    // Mobs (player + zombies)
    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, (ComponentFlag)(COMP_MOB_RENDER | COMP_TRANSFORM))) continue;

        const TransformComponent& transform  = entityManager.Transform(id);
        const MobRenderComponent& mob = entityManager.MobRender(id);

        renderer.RenderMob((int)transform.x, (int)transform.y, mob.rotation, mob.sizeMultiply, mob.type, mob.shouldDraw, mob.isMutation);
    }

    // Zombies (only)
    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, (ComponentFlag)(COMP_ZOMBIE_TAG | COMP_TRANSFORM)) || 
            entityManager.HasComponent(id, ComponentFlag(COMP_BOSS_TAG))) continue;

        const TransformComponent& transform = entityManager.Transform(id);
        const HealthComponent& hp = entityManager.Health(id);

        int hpWidth = renderer.GetHealthBarWidth();

        float healthPercentage = hp.hp / hp.maxHp;
        int fillAmount = hpWidth * healthPercentage;

        renderer.RenderHealthBar((int)transform.x - hpWidth / 2, (int)transform.y + 25, fillAmount);
    }

    // Bosses
    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, (ComponentFlag)(COMP_TRANSFORM | COMP_BOSS_TAG))) continue;

        const TransformComponent& transform = entityManager.Transform(id);
        const HealthComponent& hp = entityManager.Health(id);

        int hpWidth = renderer.GetHealthBarWidth();

        float healthPercentage = hp.hp / hp.maxHp;
        int fillAmount = hpWidth * healthPercentage;

        renderer.RenderHealthBar((int)transform.x - hpWidth / 2, (int)transform.y + 50, fillAmount);
    }

    // Bullets
    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, (ComponentFlag)(COMP_BULLET | COMP_TRANSFORM))) continue;

        const TransformComponent& transform = entityManager.Transform(id);
        renderer.RenderBullet((int)transform.x, (int)transform.y);
    }

    // Particles
    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, (ComponentFlag)(COMP_PARTICLE | COMP_TRANSFORM))) continue;

        const TransformComponent& transform = entityManager.Transform(id);
        const ParticleComponent& particle = entityManager.Particle(id);

        float t = particle.lifeTime / particle.maxLifeTime;  // 1.0 -> 0.0 as it dies
        float radius  = particle.radius * t;
        if (radius < 0.5f) continue;

        unsigned char alpha = (unsigned char)(255 * t);

        Color inner = GetColor(particle.color);
        inner.a = alpha;
        Color outer = { 140, 10, 10, alpha };  // dark red ring

        renderer.RenderParticle((int)transform.x, (int)transform.y, radius, inner, outer);
    }

    // Orbs
    for (EntityId id = 0; id < entityManager.Capacity(); ++id)
    {
        if (!entityManager.HasComponent(id, (ComponentFlag)(COMP_ORB | COMP_TRANSFORM))) continue;

        const TransformComponent& transform = entityManager.Transform(id);
        const OrbComponent& orb = entityManager.Orb(id);

        Color inner = GetColor(orb.color);
        Color outer = inner;
        outer.a = (unsigned char)(255 * 0.2);

        renderer.RenderSoulOrb((int)transform.x, (int)transform.y, orb.radius, inner, outer);
    }

    // UI
    if (world.Play)
    {
        EntityId playerID = world.GetPlayerId();

        // Difficulty
        renderer.RenderText(TextFormat("Difficulty: %.1f", world.GameDifficulty), 5, 0, 30, 0, 0x0000008F);

        // Health bar and collected soul
        if (playerID != NULL_ENTITY && entityManager.HasComponent(playerID, COMP_HEALTH))
        {
            const HealthComponent& hp = entityManager.Health(playerID);
            const PlayerComponent& playerState = entityManager.PlayerState(playerID);

            renderer.RenderText(TextFormat("HP: %.0f", hp.hp), 5, 35, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Soul: %d", playerState.collectedSoul), 5, 60, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Wave: %d", world.currentWave), 5, 85, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Time: %.0f", world.waveDuration), 500, 35, 30, 0, 0x0000008F);
            renderer.RenderText(TextFormat("Zombie(s) Left: %d", world.CountZombies()), 5, 530, 30, 0, 0x0000008F);
        }
    }
}