#include "ImageLoader.h"
#include "Config.h"

ImageLoader::ImageLoader()
{
}

ImageLoader::~ImageLoader()
{
}

void ImageLoader::LoadImage(Texture2D _texture)
{
	Image image = LoadImageFromTexture(_texture);

	mapSizeX = image.width;
	mapSizeY = image.height;
	pixelWidth = mapSizeX / WindowWidth;
	pixelHeight = mapSizeY / WindowHeight;
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
	return;
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
				color = RAYWHITE;
				break;
			}

			DrawRectangle(x * pixelWidth, y * pixelHeight, pixelWidth * scale, pixelHeight * scale, color);
		}
	}
}

void ImageLoader::SetPointAt(char _char, int mouseX, int mouseY)
{
	int mouseXnormalised = mouseX / pixelWidth;
	int mouseYnormalised = mouseY / pixelHeight;
	int placement = (mouseX * mapSizeX) + mouseY;
	if (importantPoints.find(_char) != importantPoints.end())
	{
		mapData[importantPoints[_char] / mapSizeX][importantPoints[_char] % mapSizeX] = '0';
	}
	if (mapData[placement / mapSizeX][placement % mapSizeX] == '1') return;
	importantPoints[_char] = placement;
	mapData[placement / mapSizeX][placement % mapSizeX] = _char;
}
