#include "Renderer.h"
#include "Tilemap.h"

static constexpr const char* FontPath = "Assets/Fonts/pixel-game.regular.otf";
static constexpr const char* FontBoldPath = "Assets/Fonts/pixel-game.extrude.otf";

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
    UnloadTexture(m_PlayerTexture);
    UnloadTexture(m_ZombieTexture);
    UnloadTexture(m_MutationZombieTexture);
    m_Tilemap.Unload();
}

void Renderer::Init()
{
    m_PlayerTexture = LoadTexture("Assets/Sprites/survivor_rifle.png");
    m_ZombieTexture = LoadTexture("Assets/Sprites/zombie.png");
    m_MutationZombieTexture = LoadTexture("Assets/Sprites/mutation_zombie.png");
    m_Tilemap.Load("Assets/map.csv", "Assets/ashlands_tileset.png");
}

void Renderer::SetViewOffset(int x, int y)
{
    m_ViewOffsetX = x;
    m_ViewOffsetY = y;
}

void Renderer::RenderTilemap() const
{
    m_Tilemap.Render(m_ViewOffsetX, m_ViewOffsetY, 2.5f);
}

void Renderer::RenderMob(int xp, int yp, float rotation, float multiplySize, EntityType type, bool shouldDraw, bool isMutation)
{
    if (!shouldDraw) return;

    xp -= m_ViewOffsetX;
    yp -= m_ViewOffsetY;

    const Texture2D* texture = nullptr;
    if (type == EntityType::Player) texture = &m_PlayerTexture;
    else if (type == EntityType::Zombie && !isMutation) texture = &m_ZombieTexture;
    else if (type == EntityType::Zombie && isMutation)texture = &m_MutationZombieTexture;

    if (!texture || texture->id == 0) return;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };
    Vector2 origin = { sourceRec.width * multiplySize * 0.5f, sourceRec.height * multiplySize * 0.5f };
    Rectangle destRec = { (float)xp, (float)yp, sourceRec.width * multiplySize, sourceRec.height * multiplySize };

    DrawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
}

void Renderer::RenderBullet(int xp, int yp) const
{
    xp -= m_ViewOffsetX;
    yp -= m_ViewOffsetY;

    DrawCircle(xp, yp, 3.0f, ORANGE);
    DrawCircleLines(xp, yp, 3.0f, RED);
}

void Renderer::RenderParticle(int xp, int yp, float radius, Color inner, Color outer) const
{
    xp -= m_ViewOffsetX;
    yp -= m_ViewOffsetY;

    DrawCircleV({ (float)xp, (float)yp }, radius, inner);
    DrawCircleLinesV({ (float)xp, (float)yp }, radius, outer);
}

void Renderer::RenderHealthBar(int xp, int yp, int fillAmount) const
{
    xp -= m_ViewOffsetX;
    yp -= m_ViewOffsetY;

    DrawRectangle(xp, yp, m_HealthBarWidth, m_HealthBarHeight, RED);
    DrawRectangle(xp, yp, fillAmount, m_HealthBarHeight, GREEN);
}

void Renderer::RenderSoulOrb(int xp, int yp, float radius, Color inner, Color outer) const
{
    xp -= m_ViewOffsetX;
    yp -= m_ViewOffsetY;

    DrawCircleGradient(xp, yp, radius, inner, outer);

    if (m_ShowHitboxes)
        DrawCircleLines(xp, yp, radius, RED);
}

const Texture2D& Renderer::GetTexture(EntityType type) const
{
    if (type == EntityType::Player) return m_PlayerTexture;

    return m_ZombieTexture;
}

void Renderer::RenderText(const char* text, int x, int y, int size, int style, int color)
{
    auto& fonts = style > 0 ? m_ExtrudeFonts : m_Fonts;

    if (!fonts.contains(size))
        fonts[size] = LoadFontEx(style > 0 ? FontBoldPath : FontPath, size, nullptr, 0);

    DrawTextEx(fonts.at(size), text, { (float)x, (float)y }, size, 2.0f, GetColor(color));
}

Vector2 Renderer::ScreenToWorld(Vector2 screen) const
{
    return { screen.x + (float)m_ViewOffsetX, screen.y + (float)m_ViewOffsetY };
}