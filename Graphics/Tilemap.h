#pragma once

#include <string>

class Tilemap
{
public:
	static constexpr int TILE_SIZE = 16;

	void Load(const char* csvPath, const char* tilesetPath);

	void Unload();

	void Render(int viewOffsetX, int viewOffsetY, float scale = 1.0f) const;

	int GetMapCols() const { return m_Cols; }
	int GetMapRows() const { return m_Rows; }

private:
	Rectangle TileSourceRect(int tileIndex) const;

	Texture2D m_Tileset = {};
	std::vector<int> m_Tiles;
	int m_Rows = 0;
	int m_Cols = 0;
	int m_SheetCols = 0;
};