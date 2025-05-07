#include "Application.h"

#include <iostream>
#include <fstream>
#include <cmath>

SDL_Window* Application::m_Window;
SDL_Renderer* Application::m_Renderer;

Application::Application(SDL_Window* window, SDL_Renderer* renderer, bool* running)
{
	m_Window = window;
	m_Renderer = renderer;
	m_Running = running;

	m_IsLMBPressed = false;
	m_IsRMBPressed = false;
	m_CanFill = true;
	m_CanSelect = true;

	m_CanSaveMap = false;

	m_ActiveRow = 0;
	m_ActiveCol = 0;
	m_ActiveTextureID = m_ActiveRow * 3 + m_ActiveCol;

	std::cout << "Where do you want to save the map: " << std::endl;
	std::cin >> m_MapDirectoryName;

	std::cout << "How many rows do you want:" << std::endl;
	std::cin >> m_Rows;
	std::cout << "How many cols do you want:" << std::endl;
	std::cin >> m_Cols;

	std::cout << "How many textures do you have:" << std::endl;
	std::cin >> m_TextureCount;

	if (m_TextureCount > 3 * 10) // 3 rows * 10 cols
	{
		std::cout << "Max textures is 30." << std::endl;
		std::cin.get();
		*running = false;
	}

	m_Textures = new TileTexture[m_TextureCount];

	for (int i = 0; i < m_TextureCount; i++)
	{
		std::string path;
		std::cout << "Path of texture no. " << i+1 << std::endl;
		std::cin >> path;

		m_Textures[i].id = i;
		m_Textures[i].tileTexture = IMG_LoadTexture(m_Renderer, path.c_str());

		if (m_Textures[i].tileTexture == nullptr)
		{
			std::cout << "Failed to load texture no. " << i << std::endl;
		}
	}

	m_GridAreaWidth = 1024; // 1280 * .80
	m_GridAreaHeight = 720; 
	m_TextureAreaWidth = 256; // 1280 * .20
	m_TextureAreaHeight = 720;
	m_TextureCellWidth = floor(m_TextureAreaWidth / 3); // 3 cols
	m_TextureCellHeight = floor(m_TextureAreaHeight / 10); // 10 rows

	m_CellWidth = floor(m_GridAreaWidth / m_Cols);
	m_CellHeight = floor(m_GridAreaHeight / m_Rows);

	// Need to re-calculate the grid area's width and height since some of it is lost during integer division.
	m_GridAreaWidth = m_CellWidth * m_Cols;
	m_GridAreaHeight = m_CellHeight * m_Rows;
	m_TextureAreaWidth = m_TextureCellWidth * 3;
	m_TextureAreaHeight = m_TextureCellHeight * 10;

	m_Grid = new int*[m_Rows];
	for (int i = 0; i < m_Rows; i++)
	{
		m_Grid[i] = new int[m_Cols];
		memset(m_Grid[i], -1, sizeof(int) * m_Cols);
	}

	// Textures
	m_TextureGrid = new int*[10]; // 10 rows
	for (int i = 0; i < 10; i++) 
	{
		m_TextureGrid[i] = new int[3]; // 3 cols
		memset(m_TextureGrid[i], 0, sizeof(int) * 3);
	}
}

Application::~Application()
{
}

void Application::Update()
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		if (ev.type == SDL_EVENT_QUIT)
		{
			*m_Running = false;
		}
		if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (ev.button.button == SDL_BUTTON_LEFT)
			{
				m_IsLMBPressed = true;
			}
			if (ev.button.button == SDL_BUTTON_RIGHT)
			{
				m_IsRMBPressed = true;
			}
		}
		if (ev.type == SDL_EVENT_KEY_DOWN)
		{
			if (ev.key.key == SDLK_S && m_CanSaveMap)
			{
				SaveTileMap();

				m_CanSaveMap = false;
			}
		}
		if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP)
		{
			if (ev.button.button == SDL_BUTTON_LEFT)
			{
				m_IsLMBPressed = false;
				m_CanFill = true;
			}
			if (ev.button.button == SDL_BUTTON_RIGHT)
			{
				m_IsRMBPressed = false;
			}
		}
		if (ev.type == SDL_EVENT_KEY_UP)
		{
			if (ev.key.key == SDLK_S)
			{
				m_CanSaveMap = true;
			}
		}
	}
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
	SDL_RenderClear(m_Renderer);

	SDL_GetMouseState(&m_mX, &m_mY);

	// For mouse on grid area
	if (m_mX > 0 && m_mX < m_GridAreaWidth && m_mY > 0 && m_mY < m_GridAreaHeight)
	{
		int currentRow = floor(m_mY / m_CellHeight);
		int currentCol = floor(m_mX / m_CellWidth);
		if ((currentRow != m_LastGridY || currentCol != m_LastGridX))
		{
			m_CanFill = true;
		}
		m_LastGridY = currentRow;
		m_LastGridX = currentCol;

		if (m_IsLMBPressed && m_CanFill)
		{
			m_CanFill = false;

			if (m_ActiveTextureID < m_TextureCount)
			{
				m_Grid[m_LastGridY][m_LastGridX] = m_ActiveTextureID;
			}
		}

		if (m_IsRMBPressed)
		{
			m_Grid[m_LastGridY][m_LastGridX] = -1;
		}
	}

	// For mouse on texture grid area
	// Mouse position with respect to the texture grid
	int mouseXTGrid = m_mX - m_GridAreaWidth;
	int mouseYTGrid = m_mY;

	// For mouse on texture grid area
	if (mouseXTGrid > 0 && mouseXTGrid < m_TextureAreaWidth && mouseYTGrid > 0 && mouseYTGrid < m_TextureAreaHeight)
	{
		int currentRow = floor(mouseYTGrid / m_TextureCellHeight);
		int currentCol = floor(mouseXTGrid / m_TextureCellWidth);

		if (currentRow != m_LastTGridRow || currentCol != m_LastTGridCol)
		{
			m_CanSelect = true;
		}
		m_LastTGridRow = currentRow;
		m_LastTGridCol = currentCol;

		// Check if user clicks the mouse
		if (m_IsLMBPressed && m_CanSelect)
		{
			m_ActiveRow = currentRow;
			m_ActiveCol = currentCol;

			m_ActiveTextureID = m_ActiveRow * 3 + m_ActiveCol;

			std::cout << "Texture index: " << m_ActiveTextureID << std::endl;

			m_CanSelect = false;
		}
	}
}

void Application::Render()
{
	SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);

	// Rect
	SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
	for (int i = 0; i < m_Rows; i++)
	{
		for (int j = 0; j < m_Cols; j++)
		{
			if (m_Grid[i][j] != -1)
			{
				// Render the texture to the cell
				int textureID = m_Grid[i][j];
				SDL_FRect destRect = { j * m_CellWidth, i * m_CellHeight, m_CellWidth, m_CellHeight};
				SDL_RenderTexture(m_Renderer, m_Textures[textureID].tileTexture, NULL, &destRect);
			}
		}
	}


	// Grid
	SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);
	
	for (int i = 0; i < m_Rows + 1; i++)
	{
		SDL_RenderLine(m_Renderer, 0, i * m_CellHeight, m_GridAreaWidth, i * m_CellHeight);
	}
	for (int i = 0; i < m_Cols + 1; i++)
	{
		SDL_RenderLine(m_Renderer, i * m_CellWidth, 0, i * m_CellWidth, m_GridAreaHeight);
	}

	SDL_SetRenderDrawColor(m_Renderer, 0, 255, 255, 255);
	// Texture Grid
	for (int i = 0; i < 10 + 1; i++)
	{
		SDL_RenderLine(m_Renderer, m_GridAreaWidth, i * m_TextureCellHeight, m_GridAreaWidth + m_TextureAreaWidth, i * m_TextureCellHeight);
	}
	for (int i = 0; i < 3 + 1; i++)
	{
		SDL_RenderLine(m_Renderer, m_GridAreaWidth + (i * m_TextureCellWidth), 0, m_GridAreaWidth + (i * m_TextureCellWidth), m_TextureAreaHeight);
	}

	// Textures on the grid
	int row = 0;
	int col = 0;
	for (int i = 0; i < m_TextureCount; i++)
	{
		if (i != 0 && i % 3 == 0)
		{
			row++;
			col = 0;
		}
		SDL_FRect destRect = { m_GridAreaWidth +  (col * m_TextureCellWidth), row * m_TextureCellHeight, m_TextureCellWidth, m_TextureCellHeight };
		SDL_RenderTexture(m_Renderer, m_Textures[i].tileTexture, NULL, &destRect);
		col++;
	}
	// Active texture outline
	SDL_SetRenderDrawColor(m_Renderer, 255, 255, 0, 255);
	SDL_FRect textOutline = { (m_GridAreaWidth + (m_ActiveCol * m_TextureCellWidth))-1, (m_ActiveRow * m_TextureCellHeight)+1, m_TextureCellWidth + 2, m_TextureCellHeight + 2 };
	SDL_RenderRect(m_Renderer, &textOutline);

	// Grid Area
	SDL_SetRenderDrawColor(m_Renderer, 255, 0, 255, 255);

	SDL_FRect outline = { 0, 0, m_GridAreaWidth, m_GridAreaHeight };
	SDL_RenderRect(m_Renderer, &outline);

	outline = { 1, 1, (float)(m_GridAreaWidth + 1), (float)(m_GridAreaHeight + 1) };
	SDL_RenderRect(m_Renderer, &outline);

	// Texture Grid Area
	outline = { (float)m_GridAreaWidth, 0, (float)m_TextureAreaWidth, (float)m_TextureAreaHeight };
	SDL_RenderRect(m_Renderer, &outline);

	outline = { (float)(m_GridAreaWidth), 0, (float)(m_TextureAreaWidth + 1), (float)(m_TextureAreaHeight + 1) };
	SDL_RenderRect(m_Renderer, &outline);

	SDL_RenderPresent(m_Renderer);
}

void Application::SaveTileMap()
{
	std::ofstream mapFile(m_MapDirectoryName);

	for (int i = 0; i < m_Rows; i++)
	{
		for (int j = 0; j < m_Cols; j++)
		{
			mapFile << std::string(std::to_string(m_Grid[i][j] + 1) + " ");
		}
		mapFile << "\n";
	}

	mapFile.close();
	std::cout << "Map state saved!" << std::endl;
}
