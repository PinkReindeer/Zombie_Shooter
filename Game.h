#pragma once

#include "ECS/World.h"

class Game
{
public:
    Game();

    void Update(float delta);
    void Render();

private:
    World m_World;
};