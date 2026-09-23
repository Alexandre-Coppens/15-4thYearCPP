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

	float dodgeCollisionSize;
	float alignCollisionSize;

	int gridSize;

public:
	Boids();
	~Boids();

	void Start(int _spawnNumber);
	void Update();
	void Draw();

	void CalculateGridPosition();

	std::vector<int> GridChecking(int _posX, int _posY);
	std::vector<int> CaseChecking(int _x, int _y);
	bool DistanceTesting(int _boidA, int _boidB, float _collisionSize);

	Vector2 GetDodgeDirection(int _id, std::vector<int> _neighbors);
	Vector2 GetAlignDirection(int _id, std::vector<int> _neighbors);
};