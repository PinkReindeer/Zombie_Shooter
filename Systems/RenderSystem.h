#pragma once

class World;
class Renderer;

class RenderSystem
{
public:
    void Render(const World& world, Renderer& renderer);
};
