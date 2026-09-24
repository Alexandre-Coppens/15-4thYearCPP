#pragma once
#include "raylib.h"
#include <vector>
#include <map>

class ImageLoader
{
private:
	int mapSizeX;
	int mapSizeY;
	float pixelWidth;
	float pixelHeight;
	std::vector<std::vector<char>> mapData;

	std::map<char, int> importantPoints;

public:
	ImageLoader();
	~ImageLoader();

	void LoadImage(Texture2D _texture);
	void DrawImage();

	void SetPointAt(char _char, int mouseX, int mouseY);
};