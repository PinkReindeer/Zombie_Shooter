#include "Game.h"

Game::Game()
{
    m_World.CreatePlayer(0.0f, 0.0f);
}

void Game::Update(float delta)
{
    m_World.Update(delta);
}

void Game::Render()
{
    m_World.Render();
}
