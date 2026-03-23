#pragma once

#include <memory>

#include "Graphics/Renderer.h"
#include "Game.h"

struct ApplicationConfiguration
{
	const int ScreenWidth  = 1024;
	const int ScreenHeight = 576;
	const char* Title = "Zombie Hunting";
};

class Application
{
public:
	Application(const ApplicationConfiguration& spec = ApplicationConfiguration());
	~Application();

	void Run();
	void onUpdate();
	void onRender();
public:
	static Application& Get();
	static Renderer& GetRenderer() { return Get().m_Renderer; }
private:
	void Loop();
private:
	ApplicationConfiguration m_Specification;
	Renderer m_Renderer;

	std::unique_ptr<Game> m_Game;
};