#pragma once

#include <vector>
#include <queue>

#include "ECS/ComponentTypes.h"

class EntityManager
{
public:
    EntityManager();

    EntityId CreateEntity();
    void DestroyEntity(EntityId id);
    bool IsAlive(EntityId id) const;

    void AddComponents(EntityId id, uint32_t flags);
    void RemoveComponent(EntityId id, ComponentFlag flag);
    bool HasComponent(EntityId id, ComponentFlag flag) const;

    TransformComponent& Transform (EntityId id) { return m_Transforms[id]; }
    HealthComponent& Health (EntityId id) { return m_Healths[id]; }
    MobRenderComponent& MobRender (EntityId id) { return m_MobRenders[id]; }
    PlayerComponent& PlayerState (EntityId id) { return m_PlayerStates[id]; }
    ZombieTagComponent& ZombieTag (EntityId id) { return m_ZombieTags[id]; }
    BossTagComponent& BossTag(EntityId id) { return m_BossTags[id]; }
    BulletComponent& Bullet (EntityId id) { return m_Bullets[id]; }
    ParticleComponent& Particle (EntityId id) { return m_Particles[id]; }
    OrbComponent& Orb(EntityId id) { return m_Orbs[id]; }

    const TransformComponent& Transform (EntityId id) const { return m_Transforms[id]; }
    const HealthComponent& Health (EntityId id) const { return m_Healths[id]; }
    const MobRenderComponent& MobRender (EntityId id) const { return m_MobRenders[id]; }
    const PlayerComponent& PlayerState (EntityId id) const { return m_PlayerStates[id]; }
    const BulletComponent& Bullet (EntityId id) const { return m_Bullets[id]; }
    const ParticleComponent& Particle (EntityId id) const { return m_Particles[id]; }
    const OrbComponent& Orb(EntityId id) const { return m_Orbs[id]; }

    uint32_t Capacity() const { return (uint32_t)m_ComponentMasks.size(); }

    const std::vector<uint32_t>& ComponentMasks() const { return m_ComponentMasks; }

private:
    void Grow(EntityId newCapacity);

    std::queue<EntityId> m_FreeIds;
    uint32_t m_NextId = 0;

    std::vector<uint32_t> m_ComponentMasks;

    std::vector<TransformComponent> m_Transforms;
    std::vector<HealthComponent> m_Healths;
    std::vector<MobRenderComponent> m_MobRenders;
    std::vector<PlayerComponent> m_PlayerStates;
    std::vector<ZombieTagComponent> m_ZombieTags;
    std::vector<BossTagComponent> m_BossTags;
    std::vector<BulletComponent> m_Bullets;
    std::vector<ParticleComponent> m_Particles;
    std::vector<OrbComponent> m_Orbs;
};
