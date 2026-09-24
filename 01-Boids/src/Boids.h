#pragma once
#include <vector>
#include <raylib.h>

enum BoidType
{
	None,
	Fire,
	Water,
	Grass
};

class Boids 
{
private:
	std::vector<std::vector<std::vector<int>>> _gridIndices;
	std::vector<Vector2> position, velocity;
	std::vector<BoidType> boidType;
	float size;
	float speed;

	const float dodgeCollisionSize;
	const float visionCollisionSize;
	const float attackCollisionSize;
	const float visionCone;

	const float dodgeImportance;
	const float alignImportance;
	const float cohesionImportance;
	const float chaseImportance;

	const int gridSize;

public:
	Boids();
	~Boids();

	void Start(int _spawnNumber);
	void Update();
	void Draw();

	std::vector<int> GridChecking(int _posX, int _posY);
	std::vector<int> CaseChecking(int _x, int _y);
	std::vector<int> FOVTesting(int _id, std::vector<int> _neighbors);
	std::vector<int> DistanceTesting(int _id, std::vector<int> _neighbors, float _collisionSize);

	bool CheckIfBetterType(int _atk, int _def);

	Vector2 GetDodgeDirection(int _id, std::vector<int> _neighbors);
	Vector2 GetAlignDirection(int _id, std::vector<int> _neighbors);
	Vector2 GetCohesionDirection(int _id, std::vector<int> _neighbors);
	Vector2 ChaseOtherBoids(int _id, std::vector<int> _neighbors);
	void KillOtherBoids(int _id, std::vector<int> _neighbors);
};