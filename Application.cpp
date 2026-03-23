#include "Application.h"

static Application* s_Instance = nullptr;

Application::Application(const ApplicationConfiguration& spec)
	: m_Specification(spec)
{
	s_Instance = this;
}

Application::~Application()
{
	s_Instance = nullptr;
}

Application& Application::Get()
{
	return *s_Instance;
}

void Application::Loop()
{
	onUpdate();
	onRender();
}

void Application::Run()
{
	InitWindow(m_Specification.ScreenWidth, m_Specification.ScreenHeight, m_Specification.Title);
	SetTargetFPS(5000);

	m_Renderer.Init();
	m_Game = std::make_unique<Game>();

	while (!WindowShouldClose())
	{
		Loop();
	}

	CloseWindow();
}

void Application::onUpdate()
{
	float delta = GetFrameTime();

	m_Game->Update(delta);
}

void Application::onRender()
{
	BeginDrawing();
	{
		ClearBackground(DARKGREEN);
		DrawFPS(500, 0);
		m_Game->Render();
	}
	EndDrawing();
}