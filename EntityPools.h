#pragma once

#include <vector>
#include <cstdint>
#include <cassert>

#include "GameData.h"

//  ZombiePool
struct ZombiePool
{
    std::vector<float> x, y;
    std::vector<float> rotation;
    std::vector<float> speed;
    std::vector<float> hp, maxHp;
    std::vector<float> hitboxRadius;
    std::vector<float> sizeMultiply;
    std::vector<uint8_t> isBoss;
    std::vector<uint8_t> isMutation;
    size_t count = 0;

    static constexpr size_t DefaultCapacity = 256;

    void Reserve(size_t cap = DefaultCapacity)
    {
        x.reserve(cap);
        y.reserve(cap);
        rotation.reserve(cap);
        speed.reserve(cap);
        hp.reserve(cap);
        maxHp.reserve(cap);
        hitboxRadius.reserve(cap);
        sizeMultiply.reserve(cap);
        isBoss.reserve(cap);
        isMutation.reserve(cap);
    }

    void Add(float _x, float _y, float _rotation, float _speed,
             float _hp, float _maxHp, float _hitboxRadius, float _sizeMultiply,
             uint8_t _isBoss, uint8_t _isMutation)
    {
        x.push_back(_x);
        y.push_back(_y);
        rotation.push_back(_rotation);
        speed.push_back(_speed);
        hp.push_back(_hp);
        maxHp.push_back(_maxHp);
        hitboxRadius.push_back(_hitboxRadius);
        sizeMultiply.push_back(_sizeMultiply);
        isBoss.push_back(_isBoss);
        isMutation.push_back(_isMutation);
        ++count;
    }

    void Remove(size_t i)
    {
        assert(i < count);
        size_t last = count - 1;
        if (i != last)
        {
            x[i] = x[last];
            y[i] = y[last];
            rotation[i] = rotation[last];
            speed[i] = speed[last];
            hp[i] = hp[last];
            maxHp[i] = maxHp[last];
            hitboxRadius[i] = hitboxRadius[last];
            sizeMultiply[i] = sizeMultiply[last];
            isBoss[i] = isBoss[last];
            isMutation[i] = isMutation[last];
        }
        x.pop_back();
        y.pop_back();
        rotation.pop_back();
        speed.pop_back();
        hp.pop_back();
        maxHp.pop_back();
        hitboxRadius.pop_back();
        sizeMultiply.pop_back();
        isBoss.pop_back();
        isMutation.pop_back();
        --count;
    }

    void Clear()
    {
        x.clear();
        y.clear();
        rotation.clear();
        speed.clear();
        hp.clear();
        maxHp.clear();
        hitboxRadius.clear();
        sizeMultiply.clear();
        isBoss.clear();
        isMutation.clear();
        count = 0;
    }
};

//  BulletPool
struct BulletPool
{
    std::vector<float> x, y;
    std::vector<float> vx, vy;
    std::vector<float> lifeTime;
    size_t count = 0;

    static constexpr float Speed = 600.0f;
    static constexpr float HitboxRadius = 4.0f;
    static constexpr size_t DefaultCapacity = 512;

    void Reserve(size_t cap = DefaultCapacity)
    {
        x.reserve(cap);
        y.reserve(cap);
        vx.reserve(cap);
        vy.reserve(cap);
        lifeTime.reserve(cap);
    }

    void Add(float _x, float _y, float _vx, float _vy, float _lifeTime)
    {
        x.push_back(_x);
        y.push_back(_y);
        vx.push_back(_vx);
        vy.push_back(_vy);
        lifeTime.push_back(_lifeTime);
        ++count;
    }

    void Remove(size_t i)
    {
        assert(i < count);
        size_t last = count - 1;
        if (i != last)
        {
            x[i] = x[last];
            y[i] = y[last];
            vx[i] = vx[last];
            vy[i] = vy[last];
            lifeTime[i] = lifeTime[last];
        }
        x.pop_back();
        y.pop_back();
        vx.pop_back();
        vy.pop_back();
        lifeTime.pop_back();
        --count;
    }

    void Clear()
    {
        x.clear();
        y.clear();
        vx.clear();
        vy.clear();
        lifeTime.clear();
        count = 0;
    }
};

//  ParticlePool
struct ParticlePool
{
    std::vector<float> x, y;
    std::vector<float> vx, vy;
    std::vector<float> lifeTime;
    std::vector<float> maxLifeTime;
    std::vector<float> radius;
    std::vector<uint32_t> color;
    size_t count = 0;

    static constexpr size_t DefaultCapacity = 1024;

    void Reserve(size_t cap = DefaultCapacity)
    {
        x.reserve(cap); 
        y.reserve(cap);
        vx.reserve(cap);
        vy.reserve(cap);
        lifeTime.reserve(cap);
        maxLifeTime.reserve(cap);
        radius.reserve(cap);
        color.reserve(cap);
    }

    void Add(float _x, float _y, float _vx, float _vy,
             float _lifeTime, float _maxLifeTime, float _radius, uint32_t _color)
    {
        x.push_back(_x);
        y.push_back(_y);
        vx.push_back(_vx);
        vy.push_back(_vy);
        lifeTime.push_back(_lifeTime);
        maxLifeTime.push_back(_maxLifeTime);
        radius.push_back(_radius);
        color.push_back(_color);
        ++count;
    }

    void Remove(size_t i)
    {
        assert(i < count);
        size_t last = count - 1;
        if (i != last)
        {
            x[i] = x[last];   
            y[i] = y[last];
            vx[i] = vx[last];
            vy[i] = vy[last];
            lifeTime[i] = lifeTime[last];
            maxLifeTime[i] = maxLifeTime[last];
            radius[i] = radius[last];
            color[i] = color[last];
        }
        x.pop_back();
        y.pop_back();
        vx.pop_back();
        vy.pop_back();
        lifeTime.pop_back();
        maxLifeTime.pop_back();
        radius.pop_back();
        color.pop_back();
        --count;
    }

    void Clear()
    {
        x.clear();
        y.clear();
        vx.clear();
        vy.clear();
        lifeTime.clear();
        maxLifeTime.clear();
        radius.clear();
        color.clear();
        count = 0;
    }
};

//  OrbPool
struct OrbPool
{
    std::vector<float> x, y;
    std::vector<float> radius;
    std::vector<uint32_t> color;
    std::vector<OrbType> type;
    size_t count = 0;

    static constexpr size_t DefaultCapacity = 256;

    void Reserve(size_t cap = DefaultCapacity)
    {
        x.reserve(cap);
        y.reserve(cap);
        radius.reserve(cap);
        color.reserve(cap);
        type.reserve(cap);
    }

    void Add(float _x, float _y, float _radius, uint32_t _color, OrbType _type)
    {
        x.push_back(_x);
        y.push_back(_y);
        radius.push_back(_radius);
        color.push_back(_color);
        type.push_back(_type);
        ++count;
    }

    void Remove(size_t i)
    {
        assert(i < count);
        size_t last = count - 1;
        if (i != last)
        {
            x[i] = x[last];
            y[i] = y[last];
            radius[i] = radius[last];
            color[i] = color[last];
            type[i] = type[last];
        }
        x.pop_back();
        y.pop_back();
        radius.pop_back();
        color.pop_back();
        type.pop_back();
        --count;
    }

    void Clear()
    {
        x.clear();
        y.clear();
        radius.clear();
        color.clear();
        type.clear();
        count = 0;
    }
};
