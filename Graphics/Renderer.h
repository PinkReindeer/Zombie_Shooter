#pragma once

#include <map>

#include "GameData.h"
#include "Tilemap.h"

class Renderer {
public:
  Renderer();
  ~Renderer();

  void Init();

  void SetViewOffset(int x, int y);

  void RenderTilemap() const;

  void RenderMob(int xp, int yp, float rotation, float multiplySize,
                 EntityType type, bool shouldDraw, bool isMutation);
  void RenderBullet(int xp, int yp) const;
  void RenderParticle(int xp, int yp, float radius, Color inner,
                      Color outer) const;
  void RenderHealthBar(int xp, int yp, int fillAmount) const;
  void RenderSoulOrb(int xp, int yp, float radius, Color inner,
                     Color outer) const;

  void SetShowHitboxes(bool show) { m_ShowHitboxes = show; }

  int GetHealthBarWidth() const { return m_HealthBarWidth; }
  int GetHealthBarHeight() const { return m_HealthBarHeight; }
  const Texture2D &GetTexture(EntityType type) const;

  void RenderText(const char *text, int x, int y, int size, int style,
                  int color);

  Vector2 ScreenToWorld(Vector2 screen) const;

private:
  int m_ViewOffsetX = 0, m_ViewOffsetY = 0;
  int m_HealthBarWidth = 30, m_HealthBarHeight = 4;
  bool m_ShowHitboxes = false;

  std::map<int, Font> m_Fonts, m_ExtrudeFonts;

  Texture2D m_PlayerTexture = {};
  Texture2D m_ZombieTexture = {};
  Texture2D m_MutationZombieTexture = {};

  Tilemap m_Tilemap;
};