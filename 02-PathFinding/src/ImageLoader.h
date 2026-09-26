#pragma once
#include "raylib.h"
#include <vector>
#include <map>

class ImageLoader
{
private:
	int mapSizeX;
	int mapSizeY;
	std::vector<std::vector<char>> mapData;

	std::vector<Vector2> borderWalls;
	std::map<char, int> importantPoints;

public:
	ImageLoader();
	~ImageLoader();

	std::vector<std::vector<char>> getMapData();

	void LoadBackground(Texture2D _texture);
	void SetBorderSize(int _size);
	void DrawImage();

	void SetPointAt(char _char, int mouseX, int mouseY);
	void GetBorderWalls();
	void ExpandWalls(int _size);
	void FloodFillAt(int value, int x, int y);
};