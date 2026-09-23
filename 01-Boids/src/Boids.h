#pragma once
#include <vector>
#include <raylib.h>

class Boids 
{
private:
	std::vector<std::vector<std::vector<int>>> _gridIndices;
	std::vector<Vector2> position, velocity;
	float size;
	float speed;

	const float dodgeCollisionSize;
	const float visionCollisionSize;

	const float dodgeImportance;
	const float alignImportance;
	const float cohesionImportance;

	const int gridSize;

public:
	Boids();
	~Boids();

	void Start(int _spawnNumber);
	void Update();
	void Draw();

	std::vector<int> GridChecking(int _posX, int _posY);
	std::vector<int> CaseChecking(int _x, int _y);
	std::vector<int> DistanceTesting(int _id, std::vector<int> _neighbors, float _collisionSize);

	Vector2 GetDodgeDirection(int _id, std::vector<int> _neighbors);
	Vector2 GetAlignDirection(int _id, std::vector<int> _neighbors);
	Vector2 GetCohesionDirection(int _id, std::vector<int> _neighbors);
};