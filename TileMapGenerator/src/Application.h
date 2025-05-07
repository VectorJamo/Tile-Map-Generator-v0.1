#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#include "TileTexture.h"

class Application
{
private:
	static SDL_Window* m_Window;
	static SDL_Renderer* m_Renderer;
	bool* m_Running;

	std::string m_MapDirectoryName;

	// Mouse position
	float m_mX, m_mY;
	float m_DeltaX, m_DeltaY;

	bool m_IsLMBPressed, m_IsRMBPressed;
	bool m_CanFill; // For tile map
	bool m_CanSelect; // For texture

	// Main Outline
	int m_OffsetX, m_OffsetY; 

	// Grid
	int m_GridAreaWidth, m_GridAreaHeight;
	int m_CellWidth, m_CellHeight;

	int m_Rows, m_Cols;
	int** m_Grid; // TODO: Changed boolean type to integer type that holds the texture index
	int m_LastGridX, m_LastGridY;

	// Textures
	int m_TextureCount;
	int m_TextureAreaWidth, m_TextureAreaHeight;
	int m_TextureCellWidth, m_TextureCellHeight;

	TileTexture* m_Textures;
	int** m_TextureGrid;

	int m_LastTGridRow, m_LastTGridCol;
	int m_ActiveRow, m_ActiveCol;
	int m_ActiveTextureID;

	bool m_CanSaveMap;

public:
	Application(SDL_Window* window, SDL_Renderer* renderer, bool* running);
	~Application();

	void Update();
	void Render();

	void SaveTileMap();
};

