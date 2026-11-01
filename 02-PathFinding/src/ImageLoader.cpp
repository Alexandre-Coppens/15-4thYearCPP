#include "ImageLoader.h"
#include "Benchmark.h"
#include "Config.h"
#include <iostream>
#include <string>

ImageLoader::ImageLoader()
{
}

ImageLoader::~ImageLoader()
{
}

std::vector<std::vector<char>> ImageLoader::getMapData() { return mapData; }

void ImageLoader::LoadBackground(Texture2D _texture)
{
	Benchmark::StartClock("LoadImage");
	Image image = LoadImageFromTexture(_texture);

	mapSizeX = image.width;
	mapSizeY = image.height;
	Config::pixelWidth = static_cast<float>(mapSizeX) / Config::WindowWidth;
	Config::pixelHeight = static_cast<float>(mapSizeY) / Config::WindowHeight;
	mapData = std::vector< std::vector<char>>(mapSizeX, std::vector<char>(mapSizeY, '0'));

	Color pixel;
	for (int x = 0; x < mapSizeX; x++)
	{
		for (int y = 0; y < mapSizeY; y++)
		{
			pixel = GetImageColor(image, x, y);
			mapData[x][y] = (pixel.r + pixel.g + pixel.b) > 125 ? '0' : '1';
		}
	}
	
	UnloadImage(image);
	Benchmark::StopClock("LoadImage");
	return;
}

void ImageLoader::SetBorderSize(int _size)
{
	Benchmark::StartClock("Add Border Walls");
	GetBorderWalls();
	ExpandWalls(_size);
	ChangeBordersValue();
	std::cout << "ENDED EXPANSION \n";
	Benchmark::StopClock("Add Border Walls");
}

void ImageLoader::DrawImage()
{
	for (int x = 0; x < mapSizeX; x++)
	{
		for (int y = 0; y < mapSizeY; y++)
		{
			Color color;
			float scale = 1.0f;

			switch (mapData[x][y])
			{
			case '0':
				color = RAYWHITE;
				continue;

			case '1':
				color = BLACK;
				break;

			case 's':
				color = BLUE;
				scale = 20.0f;
				break;

			case 'e':
				color = ORANGE;
				scale = 20.0f;
				break;

			default:
				color = DARKGRAY;
				break;
			}

			DrawRectangle((int)(x * Config::pixelWidth), (int)(y * Config::pixelHeight), (int)(Config::pixelWidth * scale), (int)(Config::pixelHeight * scale), color);
		}
	}
}

void ImageLoader::SetPointAt(char _char, int mouseX, int mouseY)
{
	int mouseXnormalised = (int)(static_cast<float>(mouseX) * Config::pixelWidth);
	int mouseYnormalised = (int)(static_cast<float>(mouseY) * Config::pixelHeight);
	int placement = (mouseXnormalised * mapSizeX) + mouseYnormalised;
	if (importantPoints.find(_char) != importantPoints.end())
	{
		mapData[importantPoints[_char] / mapSizeX][importantPoints[_char] % mapSizeX] = '0';
	}
	if (mapData[placement / mapSizeX][placement % mapSizeX] == '1') return;
	importantPoints[_char] = placement;
	mapData[placement / mapSizeX][placement % mapSizeX] = _char;
}

void ImageLoader::GetBorderWalls()
{
	for (int x = 0; x < mapSizeX; x++)
	{
		for (int y = 0; y < mapSizeY; y++)
		{
			if (mapData[x][y] == '1')
			{
				if (x < mapData.size() - 1)
					if (mapData[x + 1][y] != '1')
					{
                        borderWalls.push_back(Vector2{ static_cast<float>(x + 1), static_cast<float>(y) });
						continue;
					}
				if (x > 0)
					if (mapData[x - 1][y] != '1')
					{
						borderWalls.push_back(Vector2{ static_cast<float>(x - 1), static_cast<float>(y) });
						continue;
					}
				if (y < mapData[x].size() - 1)
					if (mapData[x][y + 1] != '1')
					{
						borderWalls.push_back(Vector2{ static_cast<float>(x), static_cast<float>(y + 1) });
						continue;
					}
				if (y > 0)
					if (mapData[x][y - 1] != '1')
					{
						borderWalls.push_back(Vector2{ static_cast<float>(x), static_cast<float>(y - 1) });
						continue;
					}
			}
		}
	}
}

void ImageLoader::ExpandWalls(int _size)
{
	for (Vector2 wall : borderWalls)
	{
		FloodFillAt(_size, (int)wall.x, (int)wall.y);
	}
}

void ImageLoader::FloodFillAt(int value, int x, int y)
{
	int charValue = value + 65;
	if (charValue == 65) return;
	if (x < mapData.size() - 1)
		if (mapData[x + 1][y] != '1' && mapData[x + 1][y] < charValue - 1)
		{
			mapData[x + 1][y] = charValue - 1 ;
			FloodFillAt(value - 1, x + 1, y);
		}
	if (x > 0)
		if (mapData[x - 1][y] != '1' && mapData[x - 1][y] < charValue - 1)
		{
			mapData[x - 1][y] = charValue - 1;
			FloodFillAt(value - 1, x - 1, y);
		}
	if (y < mapData[x].size() - 1)
		if (mapData[x][y + 1] != '1' && mapData[x][y + 1] < charValue - 1)
		{
			mapData[x][y + 1] = charValue - 1;
			FloodFillAt(value - 1, x, y + 1);
		}
	if (y > 0)
		if (mapData[x][y - 1] != '1' && mapData[x][y - 1] < charValue - 1)
		{
			mapData[x][y - 1] = charValue - 1;
			FloodFillAt(value - 1, x, y - 1);
		}
}

void ImageLoader::ChangeBordersValue()
{
	for (int x = 0; x < mapSizeX; x++)
	{
		for (int y = 0; y < mapSizeY; y++)
		{
			Color color;
			float scale = 1.0f;

			if (mapData[x][y] >= 65)
			{
				mapData[x][y] = '.';
			}
		}
	}
}
