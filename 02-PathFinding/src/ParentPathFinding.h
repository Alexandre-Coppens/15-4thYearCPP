#pragma once
#include <vector>
#include "raylib.h"

class ParentPathFinding
{
protected:
	std::vector<std::vector<char>> mapData;

public:
	inline void setMapData(std::vector<std::vector<char>> _newMapData) {	mapData = mapData; }
	inline std::vector<std::vector<char>>* getMapData() {	 return &mapData; }

	virtual void CalculatePathFinding() = 0;
	virtual std::vector<Vector2> GetRoadToEnd(Vector2 _start) = 0;

	virtual void Update() = 0;
	virtual void Draw() = 0;
};