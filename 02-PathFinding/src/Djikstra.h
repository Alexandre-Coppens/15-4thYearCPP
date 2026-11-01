#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "ParentPathFinding.h"


class Djikstra : public ParentPathFinding
{
private:
	std::vector<Vector2> testingThisFrame;
	std::vector<Vector2> nextCaseToTest;
	bool needToBeRebuild;
	bool needRedraw;
	Texture mTexture;

	int pixelDrawn;
	std::vector<Vector2> pathFind;

private:
	void VisitNextCase(int x, int y);

public:
	Djikstra();
	~Djikstra();

	std::vector<std::vector<char>>* getMapData();

	void CalculatePathFinding() override;
	std::vector<Vector2> GetRoadToEnd(Vector2 _start) override;

	void Update() override;
	void Draw() override;
};