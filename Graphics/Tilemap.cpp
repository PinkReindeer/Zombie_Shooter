#include <fstream>
#include <sstream>

#include "Tilemap.h"

void Tilemap::Load(const char* csvPath, const char* tilesetPath)
{
    m_Tileset = LoadTexture(tilesetPath);
    m_SheetCols = m_Tileset.width / TILE_SIZE;

    std::ifstream file(csvPath);
    if (!file.is_open())
    {
        TraceLog(LOG_ERROR, "TILEMAP: failed to open %s", csvPath);
        return;
    }

    m_Tiles.clear();
    m_Rows = 0;
    m_Cols = 0;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.find_first_not_of(" \t\r\n,") == std::string::npos) continue;

        std::istringstream ss(line);
        std::string token;
        int colCount = 0;

        while (std::getline(ss, token, ','))
        {
            auto start = token.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) continue;

            auto end = token.find_last_not_of(" \t\r\n");
            int tileId = std::stoi(token.substr(start, end - start + 1));

            m_Tiles.emplace_back(tileId);
            ++colCount;
        }

        if (m_Cols == 0)
            m_Cols = colCount;

        ++m_Rows;
    }

    TraceLog(LOG_INFO, "TILEMAP: loaded %dx%d tiles from %s", m_Cols, m_Rows, csvPath);
}

void Tilemap::Unload()
{
    UnloadTexture(m_Tileset);
    m_Tileset = {};
    m_Tiles.clear();
    m_Rows = 0;
    m_Cols = 0;
    m_SheetCols = 0;
}

Rectangle Tilemap::TileSourceRect(int tileIndex) const
{
    if (m_SheetCols <= 0) return {};

    int col = tileIndex % m_SheetCols;
    int row = tileIndex / m_SheetCols;

    return { (float)(col * TILE_SIZE), (float)(row * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };
}

void Tilemap::Render(int viewOffsetX, int viewOffsetY, float scale) const
{
    if (m_Tileset.id == 0 || m_Rows == 0 || m_Cols == 0) return;

    const int screenW  = GetScreenWidth();
    const int screenH  = GetScreenHeight();

    float scaledTileSize = TILE_SIZE * scale;

    float mapOffsetX = (m_Cols * scaledTileSize) / 2.0f;
    float mapOffsetY = (m_Rows * scaledTileSize) / 2.0f;

    int startCol = (viewOffsetX + mapOffsetX) / scaledTileSize;
    int startRow = (viewOffsetY + mapOffsetX) / scaledTileSize;
    if (startCol < 0) startCol = 0;
    if (startRow < 0) startRow = 0;

    int endCol = (viewOffsetX + mapOffsetX + screenW) / scaledTileSize + 1;
    int endRow = (viewOffsetY + mapOffsetX + screenH) / scaledTileSize + 1;
    if (endCol > m_Cols) endCol = m_Cols;
    if (endRow > m_Rows) endRow = m_Rows;

    for (int row = startRow; row < endRow; ++row)
    {
        for (int col = startCol; col < endCol; ++col)
        {
            int tileId = m_Tiles[row * m_Cols + col];
            if (tileId <= 0) continue;

            //tileId &= 0x1FFFFFFF;

            int screenX = col * scaledTileSize - mapOffsetX - viewOffsetX;
            int screenY = row * scaledTileSize - mapOffsetX - viewOffsetY;

            Rectangle src = TileSourceRect(tileId);
            Rectangle dest = { screenX, screenY, scaledTileSize, scaledTileSize };
            Vector2 origin = { 0.0f, 0.0f };

            DrawTexturePro(m_Tileset, src, dest, origin, 0.0f, WHITE);
        }
    }
}