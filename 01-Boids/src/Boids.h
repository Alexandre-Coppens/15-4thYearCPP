#pragma once
#include <vector>
#include <raylib.h>

class Boids 
{
private:
	std::vector<Vector2> position, velocity;
	float size;
	float speed;
	float collisionSize;

public:
	Boids();
	~Boids();

	void Start(int _spawnNumber);
	void Update();
	void Draw();

	bool DistanceTesting(int _boidA, int _boidB);

	Vector2 GetDodgeDirection(int _id);
};