#include <cassert>

#include "ECS/EntityManager.h"

static constexpr uint32_t INITIAL_CAPACITY = 256;

EntityManager::EntityManager()
{
    Grow(INITIAL_CAPACITY);
}

EntityId EntityManager::CreateEntity()
{
    EntityId id;

    if (!m_FreeIds.empty())
    {
        id = m_FreeIds.front();
        m_FreeIds.pop();
    }
    else
    {
        id = m_NextId++;
        if (id >= Capacity())
            Grow(Capacity() * 2);
    }

    m_ComponentMasks[id] = COMP_NONE;
    return id;
}

void EntityManager::DestroyEntity(EntityId id)
{
    assert(id < Capacity());
    m_ComponentMasks[id] = COMP_NONE;

    // Reset component data
    m_Transforms[id] = {};
    m_Healths[id] = {};
    m_MobRenders[id] = {};
    m_PlayerStates[id] = {};
    m_ZombieTags[id] = {};
    m_Bullets[id] = {};
    m_Particles[id] = {};
    m_Orbs[id] = {};

    m_FreeIds.push(id);
}

bool EntityManager::IsAlive(EntityId id) const
{
    return id < Capacity() && m_ComponentMasks[id] != COMP_NONE;
}

void EntityManager::AddComponents(EntityId id, uint32_t flags)
{
    assert(id < Capacity());
    m_ComponentMasks[id] |= flags;
}

void EntityManager::RemoveComponent(EntityId id, ComponentFlag flag)
{
    assert(id < Capacity());
    m_ComponentMasks[id] &= ~flag;
}

bool EntityManager::HasComponent(EntityId id, ComponentFlag flag) const
{
    assert(id < Capacity());
    return (m_ComponentMasks[id] & flag) == (uint32_t)flag;
}

void EntityManager::Grow(EntityId newCapacity)
{
    m_ComponentMasks.resize(newCapacity, COMP_NONE);
    m_Transforms.resize(newCapacity);
    m_Healths.resize(newCapacity);
    m_MobRenders.resize(newCapacity);
    m_PlayerStates.resize(newCapacity);
    m_ZombieTags.resize(newCapacity);
    m_Bullets.resize(newCapacity);
    m_Particles.resize(newCapacity);
    m_Orbs.resize(newCapacity);
}
