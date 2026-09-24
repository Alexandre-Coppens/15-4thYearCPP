#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"

class Djikstra
{
private:
	std::vector<std::vector<char>> mapData;
	std::vector<Vector2> testingThisFrame;
	std::vector<Vector2> nextCaseToTest;
	bool needToBeRebuild;
	int currentValue;
	Texture mTexture;
	bool needRedraw;

	int pixelDrawn;
	std::vector<Vector2> pathFind;

public:
	Djikstra();
	~Djikstra();

	void Calculate(std::vector<std::vector<char>> _newMap);
	void RoadToEnd(Vector2 _start);

	void Update();
	void Draw();

	void VisitNextCase(int x, int y);
};