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
	int currentIterations;
	Texture mTexture;
	bool needRedraw;

	int pixelDrawn;
	std::vector<Vector2> pathFind;

public:
	Djikstra(std::vector<std::vector<char>> _newMap);
	~Djikstra();

	std::vector<std::vector<char>>* getMapData();

	void Calculate(std::vector<std::vector<char>> _newMap, int _iterations);
	void RoadToEnd(Vector2 _start);

	void Update();
	void Draw();

	void VisitNextCase(int x, int y);
};